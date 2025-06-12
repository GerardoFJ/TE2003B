#!/usr/bin/env python3
"""
Tractor Dashboard Backend
Python backend with MQTT client and REST API
"""

import json
import csv
import threading
import time
from datetime import datetime
from typing import Dict, List, Optional
from dataclasses import dataclass, asdict
from collections import deque
import os
import asyncio

# FastAPI and related imports
from fastapi import FastAPI, HTTPException, WebSocket, WebSocketDisconnect
from fastapi.middleware.cors import CORSMiddleware
from fastapi.responses import FileResponse
from pydantic import BaseModel
import uvicorn

# MQTT imports
import paho.mqtt.client as mqtt

# Data models
@dataclass
class TractorData:
    velocity: float
    rpm: int
    gear: float
    timestamp: str

@dataclass
class ControlData:
    mode: str
    pedal: bool
    brake: bool
    timestamp: str

class ControlCommand(BaseModel):
    mode: str
    pedal: bool
    brake: bool

class TractorBackend:
    def __init__(self):
        self.app = FastAPI(title="Tractor Dashboard API", version="1.0.0")

        self.loop: asyncio.AbstractEventLoop = None
        self.app.on_event("startup")(self._store_event_loop)

        self.setup_cors()
        self.setup_routes()
        
        # Data storage
        self.current_data: Optional[TractorData] = None
        self.historical_data: deque = deque(maxlen=1000)  # Keep last 1000 points
        self.csv_data: List[Dict] = []
        
        # MQTT settings
        self.mqtt_broker = os.getenv('MQTT_BROKER', '192.168.0.244')
        self.mqtt_port = int(os.getenv('MQTT_PORT', '1883'))
        self.mqtt_client = None
        self.mqtt_connected = False
        
        # WebSocket connections
        self.websocket_connections: List[WebSocket] = []
        
        # Control state
        self.current_control = ControlData(
            mode="dashboard",
            pedal=False,
            brake=False,
            timestamp=datetime.now().isoformat()
        )
        
        # Start MQTT client
        self.setup_mqtt()
        
    def setup_cors(self):
        """Setup CORS middleware"""
        self.app.add_middleware(
            CORSMiddleware,
            allow_origins=["*"],
            allow_credentials=True,
            allow_methods=["*"],
            allow_headers=["*"],
        )
    
    def setup_mqtt(self):
        """Initialize MQTT client"""
        self.mqtt_client = mqtt.Client()
        self.mqtt_client.on_connect = self.on_mqtt_connect
        self.mqtt_client.on_message = self.on_mqtt_message
        self.mqtt_client.on_disconnect = self.on_mqtt_disconnect
        
        try:
            print(f"Connecting to MQTT broker at {self.mqtt_broker}:{self.mqtt_port}")
            self.mqtt_client.connect(self.mqtt_broker, self.mqtt_port, 60)
            self.mqtt_client.loop_start()
        except Exception as e:
            print(f"Failed to connect to MQTT broker: {e}")
            # Start with simulated data if MQTT fails
            self.start_simulation()
    
    def on_mqtt_connect(self, client, userdata, flags, rc):
        """MQTT connection callback"""
        if rc == 0:
            print("Connected to MQTT broker")
            self.mqtt_connected = True
            client.subscribe("tractor/data")
            print("Subscribed to tractor/data topic")
        else:
            print(f"Failed to connect to MQTT broker, return code {rc}")
            self.start_simulation()
    
    def on_mqtt_disconnect(self, client, userdata, rc):
        """MQTT disconnection callback"""
        print("Disconnected from MQTT broker")
        self.mqtt_connected = False
    
    def on_mqtt_message(self, client, userdata, msg):
        """Handle incoming MQTT messages"""
        try:
            topic = msg.topic
            payload = json.loads(msg.payload.decode())
            
            if topic == "tractor/data":
                self.process_tractor_data(payload)
                
        except Exception as e:
            print(f"Error processing MQTT message: {e}")
            
    async def _store_event_loop(self):
        """Capture uvicorn's event loop for thread-safe scheduling."""
        self.loop = asyncio.get_running_loop()

    def process_tractor_data(self, data: Dict):
        """Process incoming tractor data"""
        try:
            tractor_data = TractorData(
                velocity=float(data.get('velocity', 0)),
                rpm=int(data.get('rpm', 0)),
                gear=float(data.get('gear', 0)),
                timestamp=datetime.now().isoformat()
            )
            
            self.current_data = tractor_data
            self.historical_data.append(tractor_data)
            
            # Add to CSV data with control state
            csv_row = {
                **asdict(tractor_data),
                **asdict(self.current_control)
            }
            self.csv_data.append(csv_row)
            
            # Broadcast to WebSocket clients
            if self.loop:
                try:
                    asyncio.run_coroutine_threadsafe(
                        self.broadcast_data(),
                        self.loop
                    )
                except Exception as e:
                    print(f"Error scheduling broadcast: {e}")
            else:
                print("Warning: event loop not ready, skipping broadcast")
            
            print(f"Processed data: V={tractor_data.velocity}, RPM={tractor_data.rpm}, Gear={tractor_data.gear}")
            
        except Exception as e:
            print(f"Error processing tractor data: {e}")
    
    def start_simulation(self):
        """Start data simulation if MQTT is not available"""
        print("Starting data simulation...")
        
        def simulate_data():
            import random
            while True:
                if not self.mqtt_connected:
                    simulated_data = {
                        'velocity': random.randint(20, 70),
                        'rpm': random.randint(1000, 3000),
                        'gear': random.randint(1, 6)
                    }
                    self.process_tractor_data(simulated_data)
                time.sleep(1)
        
        simulation_thread = threading.Thread(target=simulate_data, daemon=True)
        simulation_thread.start()
    
    def publish_control_command(self, command: ControlData):
        """Publish control command via MQTT"""
        if self.mqtt_client and self.mqtt_connected:
            try:
                payload = json.dumps(asdict(command))
                self.mqtt_client.publish("tractor/control", payload)
                print(f"Published control command: {payload}")
            except Exception as e:
                print(f"Error publishing control command: {e}")
        else:
            print("MQTT not connected, control command not sent")
    
    async def broadcast_data(self):
        """Broadcast current data to all WebSocket clients"""
        if self.current_data:
            message = {
                "type": "data_update",
                "data": asdict(self.current_data),
                "control": asdict(self.current_control),
                "mqtt_connected": self.mqtt_connected
            }
            
            # Remove disconnected clients
            active_connections = []
            for websocket in self.websocket_connections:
                try:
                    print("Sending new_values")
                    await websocket.send_text(json.dumps(message))
                    active_connections.append(websocket)
                except:
                    pass  # Client disconnected
            
            self.websocket_connections = active_connections
    
    def setup_routes(self):
        """Setup FastAPI routes"""
        
        @self.app.get("/")
        async def root():
            return {"message": "Tractor Dashboard API", "version": "1.0.0"}
        
        @self.app.get("/api/status")
        async def get_status():
            """Get system status"""
            return {
                "mqtt_connected": self.mqtt_connected,
                "data_points": len(self.csv_data),
                "last_update": self.current_data.timestamp if self.current_data else None
            }
        
        @self.app.get("/api/data/current")
        async def get_current_data():
            """Get current tractor data"""
            if not self.current_data:
                raise HTTPException(status_code=404, detail="No data available")
            
            return {
                "data": asdict(self.current_data),
                "control": asdict(self.current_control),
                "mqtt_connected": self.mqtt_connected
            }
        
        @self.app.get("/api/data/historical")
        async def get_historical_data(limit: int = 50):
            """Get historical data"""
            data_list = list(self.historical_data)
            return {
                "data": [asdict(item) for item in data_list[-limit:]],
                "total_points": len(data_list)
            }
        
        @self.app.post("/api/control")
        async def send_control_command(command: ControlCommand):
            """Send control command"""
            control_data = ControlData(
                mode=command.mode,
                pedal=command.pedal,
                brake=command.brake,
                timestamp=datetime.now().isoformat()
            )
            
            self.current_control = control_data
            self.publish_control_command(control_data)
            
            return {"status": "success", "command": asdict(control_data)}
        
        @self.app.get("/api/export/csv")
        async def export_csv():
            """Export data to CSV file"""
            if not self.csv_data:
                raise HTTPException(status_code=404, detail="No data to export")
            
            filename = f"tractor_data_{datetime.now().strftime('%Y%m%d_%H%M%S')}.csv"
            filepath = f"/tmp/{filename}"
            
            try:
                with open(filepath, 'w', newline='') as csvfile:
                    if self.csv_data:
                        fieldnames = self.csv_data[0].keys()
                        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
                        writer.writeheader()
                        writer.writerows(self.csv_data)
                
                return FileResponse(
                    path=filepath,
                    filename=filename,
                    media_type='text/csv'
                )
            except Exception as e:
                raise HTTPException(status_code=500, detail=f"Error creating CSV: {str(e)}")
        
        @self.app.websocket("/ws")
        async def websocket_endpoint(websocket: WebSocket):
            """WebSocket endpoint for real-time data"""
            await websocket.accept()
            self.websocket_connections.append(websocket)
            
            try:
                # Send initial data
                if self.current_data:
                    initial_message = {
                        "type": "initial_data",
                        "data": asdict(self.current_data),
                        "control": asdict(self.current_control),
                        "mqtt_connected": self.mqtt_connected
                    }
                    await websocket.send_text(json.dumps(initial_message))
                
                # Keep connection alive
                while True:
                    await websocket.receive_text()
                    
            except WebSocketDisconnect:
                if websocket in self.websocket_connections:
                    self.websocket_connections.remove(websocket)

def main():
    """Main function to run the backend"""
    backend = TractorBackend()
    
    print("Starting Tractor Dashboard Backend...")
    print("API will be available at: http://localhost:8000")
    print("API docs at: http://localhost:8000/docs")
    print("WebSocket at: ws://localhost:8000/ws")
    
    # Run the FastAPI server
    uvicorn.run(
        backend.app,
        host="0.0.0.0",
        port=8000,
        reload=False,
        log_level="info"
    )

if __name__ == "__main__":
    main()