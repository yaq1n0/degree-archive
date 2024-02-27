
# java Controller cport replication_factor timeout rebalance_period
java Controller 12345 3 100 10000

# java Dstore cport dstore_port timeout dstore_file_folder

java Dstore 12346 12345 100 file_folder_12346
java Dstore 12347 12345 100 file_folder_12347
java Dstore 12348 12345 100 file_folder_12348

# java ClientMain cport timeout (uses client.jar)
java -cp client.jar:. ClientMain 12345 1000