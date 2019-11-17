client:
	gcc -o c.out client.c tcp_helpers.c upload_file.c

ns:
	gcc -pthread -o ns.out fileNameServer.c tcp_helpers.c fileHierarchyMethods.c

ds:
	gcc -pthread -o ds.out fileDataServer.c tcp_helpers.c
