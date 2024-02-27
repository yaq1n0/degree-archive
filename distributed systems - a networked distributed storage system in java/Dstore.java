
import java.awt.*;
import java.io.*;
import java.net.*;
import java.nio.file.Files;
import java.nio.file.Path;

class DstoreReceiver extends Thread {
    /**
     * TCP Receiver class that implements Thread.run()
     * takes in a controller as a constructor parameter
     * receives TCP messages on dstore.getport()
     * makes calls to dstore
     */

    Dstore dstore; // dstore to make method calls to

    public DstoreReceiver(Dstore dstore) {
        this.dstore = dstore;
    }

    @Override
    public void run() {
        ServerSocket ss = null;
        try {
            ss = new ServerSocket(dstore.getPort());
            while(true) {
                try {
                    Socket client = ss.accept();
                    BufferedReader in = new BufferedReader(new InputStreamReader(client.getInputStream()));
                    String line;
                    while((line = in.readLine()) != null) {
                        // what to do with each line (message) received
                        System.out.println(String.format("dstore %d: received %s", dstore.getPort(), line));
                        try {
                            String[] args = line.split(" ");
                            String command = args[0];
                            PrintWriter out = new PrintWriter(client.getOutputStream());

                            switch(command) {
                                case "STORE":
                                    // Client -> Dstore : STORE <file_name> <file_size>
                                    System.out.println(String.format("dstore %d: STORE message received", dstore.getPort()));

                                    try {
                                        String file_tostore = args[1];
                                        // only needs to work on unix so "/ is fine"
                                        String path_tostore = dstore.getFile_folder() + "/" + file_tostore;
                                        int file_size = Integer.parseInt(args[2]);

                                        // send ACK message to client
                                        System.out.println(String.format("dstore %d: sending ACK message to client", dstore.getPort()));
                                        out.println("ACK");

                                        // get client input stream and readNbytes (n = file_size)
                                        System.out.println(String.format("dstore %d: attempting to read %d bytes from client inputstream", dstore.getPort(), file_size));
                                        InputStream clientInputStream = client.getInputStream();
                                        byte[] buffer = clientInputStream.readNBytes(file_size);

                                        // write buffer to file in file_folder
                                        System.out.println(String.format("dstore %d: attempting to write data to %s", dstore.getPort(), path_tostore));
                                        File outputFile = new File(path_tostore);

                                        // initiating fileoutputstream and writing byte array contents to file
                                        OutputStream outputStream = new FileOutputStream(outputFile);
                                        outputStream.write(buffer);
                                        outputStream.close();

                                        // send STORE_ACK <file_name> to Controller
                                        System.out.println(String.format("dstore %d: sending STORE_ACK %s to controller on port %d", dstore.getPort(), file_tostore, dstore.getCport()));
                                        DstoreSender.send(dstore.getCport(), "STORE_ACK " + file_tostore);
                                    } catch (Exception e) {
                                        System.err.println("error: " + e);
                                    }

                                    break;
                                case "LOAD_DATA":
                                    // Client -> Dstore : LOAD_DATA <file_name>
                                    System.out.println(String.format("dstore %d: LOAD_DATA message received", dstore.getPort()));

                                    try {
                                        String file_toload = args[1];
                                        // only needs to work on unix so "/ is fine"
                                        String path_toload = dstore.getFile_folder() + "/" + file_toload;

                                        // read file_name to byte array
                                        System.out.println(String.format("dstore %d: attempting to read %s to byte array", dstore.getPort(), path_toload));
                                        File inputFile = new File(path_toload);
                                        byte[] bFile = new byte[(int) inputFile.length()];

                                        // initiate fileinputstream and read file contents to byte array
                                        FileInputStream inputStream = new FileInputStream(inputFile);
                                        inputStream.read(bFile);
                                        inputStream.close();

                                        // get client output stream and write
                                        OutputStream clientOutputStream = client.getOutputStream();
                                        clientOutputStream.write(bFile);
                                    } catch (Exception e) {
                                        System.err.println("error: " + e);
                                    }

                                    break;
                                case "REMOVE":
                                    // Controller -> Dstore : REMOVE <file_name>
                                    System.out.println(String.format("dstore %d: REMOVE message received", dstore.getPort()));

                                    try {
                                        String file_toremove = args[1];
                                        // only needs to work on unix so "/ is fine"
                                        String path_toremove = dstore.getFile_folder() + "/" + file_toremove;

                                        // remove file
                                        System.out.println(String.format("dstore %d: attempting to delete %s", dstore.getPort(), path_toremove));
                                        File removeFile = new File(path_toremove);
                                        if (removeFile.delete()) {
                                            System.out.println(String.format("dstore %d: successfully deleted %s", dstore.getPort(), path_toremove));
                                        } else {
                                            System.out.println(String.format("dstore %d: failed to delete %s", dstore.getPort(), path_toremove));
                                        }

                                    } catch (Exception e) {
                                        System.err.println("error: " + e);
                                    }
                                    break;
                                default:
                                    System.err.println(String.format("dstore %d: invalid command received", dstore.getPort()));
                            }

                        } catch (Exception e) {
                            System.err.println(String.format("dstore %d: invalid message received", dstore.getPort()));
                            System.err.println("error: " + e);
                        }
                    }
                    client.close();
                } catch(Exception e) { System.err.println("error: " + e); }
            }
        }
        catch(Exception e) {
            System.err.println("error: " + e);
        }
        finally {
            if (ss != null)
                try {
                    ss.close();
                } catch (IOException e) {
                    System.err.println("error: " + e);
                }
        }
    }
}

class DstoreSender {
    /**
     * TCP Sender class
     * has a static .send(int port, String message) method
     */

    public static void send(int port, String message) {
        Socket socket = null;
        try {
            InetAddress address = InetAddress.getLocalHost(); // send tcp message locally
            socket = new Socket(address, port);
            PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
            System.out.println(String.format("dstore sending to port %d: %s", port, message));
            out.println(message); // TCP message to send
        }
        catch(Exception e) {
            System.err.println("error: " + e);
        }
        finally {
            if (socket != null) {
                try {
                    socket.close();
                } catch (IOException e) {
                    System.err.println("error: " + e);
                }
            }
        }
    }
}

public class Dstore {
    public int port; // dstore port
    public int cport; // controller port
    public int timeout; // timeout duration in seconds

    public String file_folder; // folder for the dstore object to store files

    public DstoreReceiver receiver;

    public static void main(String[] args) {
        // pass through args from static call to new dstore constructor
        Dstore dstore = new Dstore(args);
    }

    public Dstore(String[] args) {
        // construct dstore object from args
        this.port = Integer.parseInt(args[0]);
        this.cport = Integer.parseInt(args[1]);
        this.timeout = Integer.parseInt(args[2]);
        this.file_folder = args[3];

        System.out.println(String.format("dstore %d: initializing new Dstore", port));
        System.out.println(String.format("dstore %d: set timeout to %d", port, timeout));

        File dir = new File(file_folder);
        if (!dir.exists()) {
            dir.mkdirs();
            System.out.println(String.format("dstore %d: created file_folder at %s", port, file_folder));
        } else {
            cleanDir(file_folder); // clean the dstore folder
        }

        receiver = new DstoreReceiver(this);
        receiver.start();

        System.out.println(String.format("dstore %d: attempting to join controller at %d", port, cport));
        String message = "JOIN " + port;
        DstoreSender.send(cport, message);;
    }

    private void cleanDir(String path) {
        try {
            File dir = new File(path);
            File[] files = dir.listFiles();
            // only deletes files, not subdirectories
            for (File file : files) {
                file.delete();
                }
            System.out.println("dstore: cleaned directory: " + path);
        } catch (Exception e) {
            System.out.println(e.getMessage());
        }
    }

    public int getPort() {
        return port;
    }

    public int getCport() {
        return cport;
    }

    public int getTimeout() {
        return timeout;
    }

    public String getFile_folder() {
        return file_folder;
    }

}