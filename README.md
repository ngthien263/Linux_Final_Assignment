make clean
make all

# Run gateway server on a given port
./bin/sensor_gateway <port>

# Run sensor node client connecting to server
./bin/sensor_node <ip> <port>
```

Replace `<port>` and `<ip>` with actual values, e.g., `127.0.0.1 8888`.

Issues
- **Only one sensor is processed at a time**
  - This is due to a different in data structure between sender (sensor_node) and reader (gateway).
