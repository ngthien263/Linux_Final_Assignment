.PHONY := all clean
all: 
	gcc -o sensor_gateway sensor_gateway.c -pthread
	gcc -o sensor_node sensor_node.c

clean:
	rm -rf logfifo
	rm -rf myfifo
	rm -rf gateway.log
	rm -rf sensor_gateway
	rm -rf sensor_node 
