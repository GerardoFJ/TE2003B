'use client'
import React, { useState, useEffect, useRef } from 'react';
import { LineChart, Line, XAxis, YAxis, CartesianGrid, Tooltip, ResponsiveContainer, Legend } from 'recharts';
import { Power, Download, Settings, Gauge, Zap, Cog } from 'lucide-react';

const TractorDashboard = () => {
  // State management
  const [isConnected, setIsConnected] = useState(false);
  const [currentData, setCurrentData] = useState({ velocity: 0, rpm: 0, gear: 0 });
  const [historicalData, setHistoricalData] = useState([]);
  const [isDashboardMode, setIsDashboardMode] = useState(true);
  const [isManualMode, setIsManualMode] = useState(false);
  const [pedal, setPedal] = useState(false);
  const [brake, setBrake] = useState(false);
  const [backendConnected, setBackendConnected] = useState(false);
  
  // WebSocket and API references
  const wsRef = useRef(null);
  const reconnectTimeoutRef = useRef(null);
  const API_BASE_URL = 'http://localhost:8000';

  // WebSocket connection and data fetching
  useEffect(() => {
    connectWebSocket();
    fetchCurrentData();
    fetchHistoricalData();
    
    return () => {
      if (wsRef.current) {
        wsRef.current.close();
      }
      if (reconnectTimeoutRef.current) {
        clearTimeout(reconnectTimeoutRef.current);
      }
    };
  }, []);

  const connectWebSocket = () => {
    try {
      const ws = new WebSocket('ws://localhost:8000/ws');
      wsRef.current = ws;

      ws.onopen = () => {
        console.log('WebSocket connected');
        setBackendConnected(true);
        if (reconnectTimeoutRef.current) {
          clearTimeout(reconnectTimeoutRef.current);
        }
      };

      ws.onmessage = (event) => {
        try {
          const message = JSON.parse(event.data);
          
          if (message.type === 'data_update' || message.type === 'initial_data') {
            const newData = {
              ...message.data,
              timestamp: new Date(message.data.timestamp).toLocaleTimeString()
            };
            
            setCurrentData(newData);
            setIsConnected(message.mqtt_connected);
            
            // Update control state
            if (message.control) {
              setPedal(message.control.pedal);
              setBrake(message.control.brake);
              setIsDashboardMode(message.control.mode === 'dashboard');
              setIsManualMode(message.control.mode === 'manual');
            }
            
            // Add to historical data (keep last 50 points)
            setHistoricalData(prev => {
              const updated = [...prev, { ...newData, time: Date.now() }];
              return updated.slice(-50);
            });
          }
        } catch (error) {
          console.error('Error parsing WebSocket message:', error);
        }
      };

      ws.onclose = () => {
        console.log('WebSocket disconnected');
        setBackendConnected(false);
        
        // Attempt to reconnect after 3 seconds
        reconnectTimeoutRef.current = setTimeout(() => {
          console.log('Attempting to reconnect...');
          connectWebSocket();
        }, 3000);
      };

      ws.onerror = (error) => {
        console.error('WebSocket error:', error);
        setBackendConnected(false);
      };

    } catch (error) {
      console.error('Failed to connect WebSocket:', error);
      setBackendConnected(false);
    }
  };

  const fetchCurrentData = async () => {
    try {
      const response = await fetch(`${API_BASE_URL}/api/data/current`);
      if (response.ok) {
        const result = await response.json();
        setCurrentData({
          ...result.data,
          timestamp: new Date(result.data.timestamp).toLocaleTimeString()
        });
        setIsConnected(result.mqtt_connected);
      }
    } catch (error) {
      console.error('Error fetching current data:', error);
    }
  };

  const fetchHistoricalData = async () => {
    try {
      const response = await fetch(`${API_BASE_URL}/api/data/historical?limit=50`);
      if (response.ok) {
        const result = await response.json();
        const formattedData = result.data.map(item => ({
          ...item,
          timestamp: new Date(item.timestamp).toLocaleTimeString(),
          time: new Date(item.timestamp).getTime()
        }));
        setHistoricalData(formattedData);
      }
    } catch (error) {
      console.error('Error fetching historical data:', error);
    }
  };

  // Handle mode switch and send to backend
  const handleModeSwitch = async (mode) => {
    const newMode = mode === 'dashboard';
    setIsDashboardMode(newMode);
    setIsManualMode(!newMode);
    
    await sendControlCommand({
      mode: mode,
      pedal,
      brake
    });
  };

  // Handle pedal/brake controls
  const handlePedal = async (pressed) => {
    setPedal(pressed);
    const newBrake = pressed ? false : brake; // Mutual exclusion
    setBrake(newBrake);
    
    await sendControlCommand({
      mode: isDashboardMode ? 'dashboard' : 'manual',
      pedal: pressed,
      brake: newBrake
    });
  };

  const handleBrake = async (pressed) => {
    setBrake(pressed);
    const newPedal = pressed ? false : pedal; // Mutual exclusion
    setPedal(newPedal);
    
    await sendControlCommand({
      mode: isDashboardMode ? 'dashboard' : 'manual',
      pedal: newPedal,
      brake: pressed
    });
  };

  const sendControlCommand = async (command) => {
    try {
      const response = await fetch(`${API_BASE_URL}/api/control`, {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify(command),
      });
      
      if (!response.ok) {
        console.error('Failed to send control command');
      }
    } catch (error) {
      console.error('Error sending control command:', error);
    }
  };

  // Export to CSV via backend
  const exportToCSV = async () => {
    try {
      const response = await fetch(`${API_BASE_URL}/api/export/csv`);
      
      if (response.ok) {
        const blob = await response.blob();
        const url = window.URL.createObjectURL(blob);
        const a = document.createElement('a');
        a.href = url;
        a.download = `tractor_data_${new Date().toISOString().split('T')[0]}.csv`;
        document.body.appendChild(a);
        a.click();
        document.body.removeChild(a);
        window.URL.revokeObjectURL(url);
      } else {
        alert('No data to export or server error');
      }
    } catch (error) {
      console.error('Error exporting CSV:', error);
      alert('Failed to export CSV');
    }
  };

  return (
    <div className="min-h-screen bg-gradient-to-br from-gray-900 via-gray-800 to-green-900 p-4">
      <div className="max-w-9xl mx-auto">
        {/* Header */}
        <div className="bg-gray-800 rounded-lg p-6 mb-6 shadow-2xl border border-gray-700">
          <div className="flex flex-col xl:flex-row justify-between items-center gap-4">
            <div className="flex items-center space-x-4">
              <Cog className="w-8 h-8 text-green-400" />
              <h1 className="text-3xl font-bold text-white">Tractor Dashboard</h1>
              <div className={`flex items-center space-x-2 px-3 py-1 rounded-full ${
                backendConnected && isConnected ? 'bg-green-600' : 'bg-red-600'
              }`}>
                <div className={`w-2 h-2 rounded-full ${
                  backendConnected && isConnected ? 'bg-green-300 animate-pulse' : 'bg-red-300'
                }`}></div>
                <span className="text-white text-sm">
                  {backendConnected ? (isConnected ? 'MQTT Connected' : 'MQTT Disconnected') : 'Backend Offline'}
                </span>
              </div>
            </div>
            
            <div className="flex items-center space-x-4">
              {/* Mode Switch */}
              <div className="flex bg-gray-700 rounded-lg p-1">
                <button
                  onClick={() => handleModeSwitch('dashboard')}
                  className={`px-4 py-2 rounded-md transition-all ${
                    isDashboardMode 
                      ? 'bg-green-600 text-white shadow-lg' 
                      : 'text-gray-300 hover:text-white'
                  }`}
                >
                  Manual
                </button>
                <button
                  onClick={() => handleModeSwitch('manual')}
                  className={`px-4 py-2 rounded-md transition-all ${
                    isManualMode 
                      ? 'bg-blue-600 text-white shadow-lg' 
                      : 'text-gray-300 hover:text-white'
                  }`}
                >
                  Dashboard
                </button>
              </div>
              
              <button
                onClick={exportToCSV}
                className="flex items-center space-x-2 bg-purple-600 hover:bg-purple-700 text-white px-4 py-2 rounded-lg transition-all transform hover:scale-105"
              >
                <Download className="w-4 h-4" />
                <span>Export CSV</span>
              </button>
            </div>
          </div>
        </div>

        {/* Control Panel */}
        <div className="bg-gray-800 rounded-lg p-6 mb-6 shadow-2xl border border-gray-700">
          <h3 className="text-xl font-bold text-white mb-4 text-center">Control Panel</h3>
          <div className="flex justify-center space-x-8">
            <button
              onMouseDown={() => handlePedal(true)}
              onMouseUp={() => handlePedal(false)}
              onMouseLeave={() => handlePedal(false)}
              className={`flex items-center space-x-3 px-8 py-4 rounded-lg font-bold text-lg transition-all transform ${
                pedal 
                  ? 'bg-green-500 text-white scale-105 shadow-lg shadow-green-500/50' 
                  : 'bg-gray-700 text-gray-300 hover:bg-gray-600 hover:scale-105'
              }`}
              disabled={!isManualMode}
            >
              <Power className="w-6 h-6" />
              <span>PEDAL</span>
            </button>
            
            <button
              onMouseDown={() => handleBrake(true)}
              onMouseUp={() => handleBrake(false)}
              onMouseLeave={() => handleBrake(false)}
              className={`flex items-center space-x-3 px-8 py-4 rounded-lg font-bold text-lg transition-all transform ${
                brake 
                  ? 'bg-red-500 text-white scale-105 shadow-lg shadow-red-500/50' 
                  : 'bg-gray-700 text-gray-300 hover:bg-gray-600 hover:scale-105'
              }`}
              disabled={!isManualMode}
            >
              <Power className="w-6 h-6" />
              <span>BRAKE</span>
            </button>
          </div>
          
          {!isManualMode && (
            <p className="text-center text-gray-400 mt-4">
              Switch to Dashboard mode to enable controls
            </p>
          )}
        </div>

        {/* Current Values Display */}
        <div className="grid grid-cols-1 md:grid-cols-3 xl:grid-cols-3 gap-6 mb-6">
          <div className="bg-gradient-to-br from-blue-600 to-blue-800 rounded-lg p-6 shadow-2xl">
            <div className="flex items-center justify-between">
              <div>
                <p className="text-blue-200 text-sm uppercase tracking-wide">Velocity</p>
                <p className="text-4xl font-bold text-white">{currentData.velocity}</p>
                <p className="text-blue-200 text-sm">km/h</p>
              </div>
              <Gauge className="w-12 h-12 text-blue-200" />
            </div>
          </div>
          
          <div className="bg-gradient-to-br from-red-600 to-red-800 rounded-lg p-6 shadow-2xl">
            <div className="flex items-center justify-between">
              <div>
                <p className="text-red-200 text-sm uppercase tracking-wide">RPM</p>
                <p className="text-4xl font-bold text-white">{currentData.rpm.toLocaleString()}</p>
                <p className="text-red-200 text-sm">revolutions/min</p>
              </div>
              <Zap className="w-12 h-12 text-red-200" />
            </div>
          </div>
          
          <div className="bg-gradient-to-br from-green-600 to-green-800 rounded-lg p-6 shadow-2xl">
            <div className="flex items-center justify-between">
              <div>
                <p className="text-green-200 text-sm uppercase tracking-wide">Gear</p>
                <p className="text-4xl font-bold text-white">{currentData.gear}</p>
                <p className="text-green-200 text-sm">current gear</p>
              </div>
              <Settings className="w-12 h-12 text-green-200" />
            </div>
          </div>
        </div>

        {/* Charts */}
        <div className="grid grid-cols-1 xl:grid-cols-3 gap-6 mb-6">
          {/* Velocity Chart */}
          <div className="bg-gray-800 rounded-lg p-6 shadow-2xl border border-gray-700">
            <h3 className="text-xl font-bold text-white mb-4">Velocity Trend</h3>
            <ResponsiveContainer width="100%" height={300}>
              <LineChart data={historicalData}>
                <CartesianGrid strokeDasharray="3 3" stroke="#374151" />
                <XAxis 
                  dataKey="timestamp" 
                  stroke="#9CA3AF"
                  fontSize={12}
                />
                <YAxis stroke="#9CA3AF" fontSize={12} />
                <Tooltip 
                  contentStyle={{ 
                    backgroundColor: '#1F2937', 
                    border: '1px solid #374151',
                    borderRadius: '8px',
                    color: '#FFFFFF'
                  }} 
                />
                <Legend />
                <Line 
                  type="monotone" 
                  dataKey="velocity" 
                  stroke="#3B82F6" 
                  strokeWidth={3}
                  name="Velocity (km/h)"
                  dot={false}
                />
              </LineChart>
            </ResponsiveContainer>
          </div>
          
          {/* RPM Chart */}
          <div className="bg-gray-800 rounded-lg p-6 shadow-2xl border border-gray-700">
            <h3 className="text-xl font-bold text-white mb-4">RPM Trend</h3>
            <ResponsiveContainer width="100%" height={300}>
              <LineChart data={historicalData}>
                <CartesianGrid strokeDasharray="3 3" stroke="#374151" />
                <XAxis 
                  dataKey="timestamp" 
                  stroke="#9CA3AF"
                  fontSize={12}
                />
                <YAxis stroke="#9CA3AF" fontSize={12} />
                <Tooltip 
                  contentStyle={{ 
                    backgroundColor: '#1F2937', 
                    border: '1px solid #374151',
                    borderRadius: '8px',
                    color: '#FFFFFF'
                  }} 
                />
                <Legend />
                <Line 
                  type="monotone" 
                  dataKey="rpm" 
                  stroke="#EF4444" 
                  strokeWidth={3}
                  name="RPM"
                  dot={false}
                />
              </LineChart>
            </ResponsiveContainer>
          </div>
          
          {/* Gear Chart */}
          <div className="bg-gray-800 rounded-lg p-6 shadow-2xl border border-gray-700">
            <h3 className="text-xl font-bold text-white mb-4">Gear Changes</h3>
            <ResponsiveContainer width="100%" height={300}>
              <LineChart data={historicalData}>
                <CartesianGrid strokeDasharray="3 3" stroke="#374151" />
                <XAxis 
                  dataKey="timestamp" 
                  stroke="#9CA3AF"
                  fontSize={12}
                />
                <YAxis stroke="#9CA3AF" fontSize={12} domain={[0, 6]} />
                <Tooltip 
                  contentStyle={{ 
                    backgroundColor: '#1F2937', 
                    border: '1px solid #374151',
                    borderRadius: '8px',
                    color: '#FFFFFF'
                  }} 
                />
                <Legend />
                <Line 
                  type="stepAfter" 
                  dataKey="gear" 
                  stroke="#10B981" 
                  strokeWidth={3}
                  name="Current Gear"
                  dot={{ fill: '#10B981', strokeWidth: 2, r: 4 }}
                />
              </LineChart>
            </ResponsiveContainer>
          </div>
        </div>

        {/* Status Footer */}
        <div className="mt-6 text-center text-gray-400">
          <p>Mode: <span className="text-white font-semibold">
            {isDashboardMode ? 'Manual' : 'Dashboard'}
          </span> | Backend: <span className={`font-semibold ${backendConnected ? 'text-green-400' : 'text-red-400'}`}>
            {backendConnected ? 'Connected' : 'Disconnected'}
          </span> | MQTT: <span className={`font-semibold ${isConnected ? 'text-green-400' : 'text-red-400'}`}>
            {isConnected ? 'Connected' : 'Disconnected'}
          </span> | Last Update: <span className="text-white font-semibold">
            {currentData.timestamp}
          </span></p>
        </div>
      </div>
    </div>
  );
};

export default TractorDashboard;