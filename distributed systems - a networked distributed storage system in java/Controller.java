import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.security.spec.ECField;
import java.util.ArrayList;
import java.util.HashMap;

class ControllerReceiver extends Thread {
    /**
     * TCP Receiver class that implements Thread.run()
     * takes in a controller as a constructor parameter
     * receives TCP messages on controller.getCport()
     * makes calls to controller
     */

    Controller controller; // controller to make method calls to

    public ControllerReceiver(Controller controller) {
        this.controller = controller;

        ServerSocket ss = null;

        try {
            ss = new ServerSocket(controller.getCport());
            while(true) {
                try {
                    Socket client = ss.accept();
                    ControllerServiceThread serviceThread = new ControllerServiceThread(controller, client);
                    Thread _serviceThread = new Thread(serviceThread);
                    _serviceThread.start();
                } catch (Exception e) {
                    System.err.println("error: " + e);
                }
            }
        } catch (Exception e) {
            System.err.println("error: " + e);
        }
    }
}

class ControllerServiceThread implements Runnable{
    Controller controller;
    Socket client;

    public ControllerServiceThread(Controller controller, Socket client) {
        this.controller = controller;
        this.client = client;
    }

    @Override
    public void run() {
        try {
            BufferedReader in = new BufferedReader(new InputStreamReader(client.getInputStream()));
            String line;
            while ((line = in.readLine()) != null) {
                // what to do with each line (message) received
                System.out.println(String.format("controller %d: received %s", controller.getCport(), line));
                try {
                    String[] args = line.split(" ");
                    String command = args[0];
                    PrintWriter out = new PrintWriter(client.getOutputStream());

                    switch(command) {
                        case "JOIN":
                            // Dstore -> Controller : JOIN <dstore_port>
                            System.out.println(String.format("controller %d: JOIN message received", controller.getCport()));

                            try {
                                int dstore_port = Integer.parseInt(args[1]);

                                System.out.println(String.format("controller %d: attempting to add dstore %d to index", controller.getCport(), dstore_port));
                                controller.getIndex().addDstore(dstore_port);
                            } catch (Exception e) {
                                System.err.println("error " + e);
                            }

                            if (controller.getIndex().listDstores().size() >= controller.getR()) {
                                // if the index contains enough dstores, set enoughDstores to true to enable operations.
                                controller.enoughDstores = true;
                            }

                            break;
                        case "STORE":
                            // TODO: solve this for multiple clients
                            // Client -> Controller : STORE <file_name> <file_size>
                            System.out.println(String.format("controller %d: STORE message received", controller.getCport()));

                            if (controller.enoughDstores) {
                                try {
                                    String file_tostore = args[1];
                                    int file_size = Integer.parseInt(args[2]); // this doesn't do anything without rebalancing

                                    System.out.println(String.format("controller %d: attempting to store %s, size = %d", controller.getCport(), file_tostore, file_size));

                                    String storeto_message = "STORE_TO";
                                    ArrayList<Integer> dstores = controller.getIndex().listDstores();

                                    // just taking the first R dstore_ports that come up when listDstores()
                                    for (int i = 0; i < controller.getR(); i++) {
                                        int dstore_port = dstores.get(i);
                                        storeto_message += (" " + dstore_port);

                                        ArrayList<DstoreFile> dstore_Files = controller.getIndex().pairs.get(dstore_port);
                                        if (!dstore_Files.contains(file_tostore)) {
                                            System.out.println(String.format("controller %d: add %s to index", controller.getCport(), file_tostore));
                                            controller.getIndex().addFile(dstore_port, file_tostore);
                                        } else {
                                            System.err.println(String.format("controller %d: file %s already exists in dstore %d", controller.getCport(), file_tostore, dstore_port));
                                            out.println("ERROR_FILE_ALREADY_EXISTS");
                                        }

                                        String status = "store in progress";
                                        System.out.println(String.format("controller %d: set %s status to %s", controller.getCport(), file_tostore, status));
                                        controller.getIndex().setAllFileStatus(file_tostore, status);
                                    }

                                    System.out.println(String.format("controller %d: sent %s", controller.getCport(), storeto_message));
                                    out.println(storeto_message);
                                } catch (Exception e) {
                                    System.err.println("error: " + e);
                                }
                            } else {
                                System.err.println(String.format("controller %d: not enough dstores to execute STORE operation", controller.getCport()));
                                out.println("ERROR_NOT_ENOUGH_DSTORES");
                            }

                            break;
                        case "LOAD":
                            // Client -> Controller : LOAD <file_name>
                            System.out.println(String.format("controller %d: LOAD message received", controller.getCport()));
                            if (controller.enoughDstores) {
                                try {
                                    String file_toload = args[1];

                                    System.out.println(String.format("controller %d: attempting to load %s", controller.getCport(), file_toload));

                                    if (controller.getIndex().listFileNames().contains(file_toload)) {
                                        // check if requested file exists
                                        ArrayList<Integer> possible_dstores = new ArrayList<>();

                                        for (Integer dstore : controller.getIndex().pairs.keySet()) {
                                            // for all dstores
                                            for (DstoreFile dstoreFile : controller.getIndex().pairs.get(dstore)) {
                                                // for all dstorefiles in each dstore
                                                if (dstoreFile.getName().equals(file_toload)) {
                                                    // add possible dstore
                                                    possible_dstores.add(dstore);
                                                }
                                            }
                                        }

                                        // TODO: this might need to be changed, it currently just takes the first dstore
                                        String load_message = "LOAD_FROM " + possible_dstores.get(0);
                                        System.out.println(String.format("controller %d: sent %s", controller.getCport(), load_message));
                                        out.println(load_message);
                                    } else {
                                        System.err.println(String.format("controller %d: file %s doesn't exist in any dstore", controller.getCport(), file_toload));
                                        out.println("ERROR_FILE_DOES_NOT_EXIST");
                                    }

                                } catch (Exception e) {
                                    System.err.println("error: " + e);
                                }
                            } else {
                                System.err.println(String.format("controller %d: not enough dstores to execute LOAD operation", controller.getCport()));
                                out.println("ERROR_NOT_ENOUGH_DSTORES");
                            }
                            break;
                        case "REMOVE":
                            // TODO: solve this for multiple clients
                            // Client -> Controller : REMOVE <file_name>
                            System.out.println(String.format("controller %d: REMOVE message received", controller.getCport()));
                            if (controller.enoughDstores) {
                                try {
                                    String file_toremove = args[1];

                                    System.out.println(String.format("controller %d: attempting to remove %s", controller.getCport(), file_toremove));

                                    ArrayList<Integer> dstores_toRemoveFrom = new ArrayList<>();

                                    for (Integer dstore : controller.getIndex().pairs.keySet()) {
                                        // for all dstores
                                        for (DstoreFile dstoreFile : controller.getIndex().pairs.get(dstore)) {
                                            // for all dstorefiles in each dstore
                                            if (dstoreFile.getName().equals(file_toremove)) {
                                                // add dstore where remove operation has to occur
                                                dstores_toRemoveFrom.add(dstore);
                                                dstoreFile.setStatus("remove in progress");
                                            }
                                        }
                                    }

                                    for (Integer dstore : dstores_toRemoveFrom) {
                                        // TODO: timeout?
                                        String remove_message = "REMOVE " + file_toremove;
                                        ControllerSender.send(dstore, remove_message);
                                    }

                                } catch (Exception e) {
                                    System.out.println("error: " + e);
                                }
                            } else {
                                System.err.println(String.format("controller %d: not enough dstores to execute REMOVE operation", controller.getCport()));
                                out.println("ERROR_NOT_ENOUGH_DSTORES");
                            }
                            break;
                        case "LIST":
                            // Client -> Controller : LIST
                            System.out.println(String.format("controller %d: LIST message received", controller.getCport()));

                            String list_message = "LIST";

                            for (String file : controller.getIndex().listFileNames()) {
                                list_message += (" " + file);
                            }

                            System.out.println(String.format("controller %d: sent %s", controller.getCport(), list_message));
                            out.println(list_message);

                            break;
                        case "STORE_ACK":
                            // Dstore -> Controller : STORE_ACK file_name
                            System.out.println(String.format("controller %d: STORE_ACK message received", controller.getCport()));

                            try {
                                String file_storeAck = args[1];

                                if (controller.getIndex().listFileNames().contains(file_storeAck)) {
                                    controller.getIndex().setFileStatus(client.getPort(), file_storeAck, "store complete");

                                    boolean allStored = true;
                                    for (Integer dstore : controller.getIndex().pairs.keySet()) {
                                        // for all dstores
                                        ArrayList<DstoreFile> dstoreFiles = controller.getIndex().pairs.get(dstore);
                                        for (DstoreFile dstoreFile : dstoreFiles) {
                                            // for all dstorefiles
                                            if (dstoreFile.getName().equals(file_storeAck)) {
                                                allStored = dstoreFile.getStatus().equals("store complete");
                                            }
                                        }
                                    }

                                    if (allStored) {
                                        System.out.println(String.format("controller %d: sending STORE_COMPLETE", controller.getCport()));
                                        // send "STORE_COMPLETE" to Client
                                    }
                                } else {
                                    System.err.println(String.format("controller %d: file %s doesn't exist in any dstore", controller.getCport(), file_storeAck));
                                    out.println("ERROR_FILE_DOES_NOT_EXIST");
                                }
                            } catch (Exception e) {
                                System.err.println("error: " + e);
                            }
                            break;
                        case "REMOVE_ACK":
                            // Dstore -> Controller : REMOVE_ACK file_name
                            System.out.println(String.format("controller %d: REMOVE_ACK message received", controller.getCport()));

                            try {
                                String file_removeAck = args[1];

                                if (controller.getIndex().listFileNames().contains(file_removeAck)) {
                                    controller.getIndex().setFileStatus(client.getPort(), file_removeAck, "remove complete");

                                    boolean allRemoved = true;
                                    for (Integer dstore : controller.getIndex().pairs.keySet()) {
                                        // for all dstores
                                        ArrayList<DstoreFile> dstoreFiles = controller.getIndex().pairs.get(dstore);
                                        for (DstoreFile dstoreFile : dstoreFiles) {
                                            // for all dstorefiles
                                            if (dstoreFile.getName().equals(file_removeAck)) {
                                                allRemoved = dstoreFile.getStatus().equals("remove complete");
                                            }
                                        }
                                    }

                                    if (allRemoved) {
                                        // removing files
                                        for (Integer dstore : controller.getIndex().pairs.keySet()) {
                                            // for all dstores
                                            ArrayList<DstoreFile> dstoreFiles = controller.getIndex().pairs.get(dstore);
                                            for (DstoreFile dstoreFile : dstoreFiles) {
                                                // for all dstorefiles
                                                if (dstoreFile.getName().equals(file_removeAck) && dstoreFile.getStatus().equals("remove complete")) {
                                                    controller.getIndex().removeFileFromAll(file_removeAck);
                                                }
                                            }
                                        }

                                        System.out.println(String.format("controller %d: sending REMOVE_COMPLETE", controller.getCport()));
                                        // send "REMOVE_COMPLETE" to Client
                                    }
                                } else {
                                    System.err.println(String.format("controller %d: file %s doesn't exist in any dstore", controller.getCport(), file_removeAck));
                                    out.println("ERROR_FILE_DOES_NOT_EXIST");
                                }
                            } catch (Exception e) {
                                System.err.println("error: " + e);
                            }
                            break;
                        default:
                            System.err.println(String.format("controller %d: invalid command received", controller.getCport()));
                    }

                } catch (Exception e) {
                    System.err.println(String.format("controller %d: invalid message received", controller.getCport()));
                    System.err.println("error: " + e);
                }
            }
            client.close();
        } catch (Exception e) {
            System.err.println(String.format("controller %d: exception in service thread", controller.getCport()));
        }
    }
}
class ControllerSender {
    /**
     * TCP Sender class
     * has a static .send(int port, String message) method
     */

    public static void send(int port, String message) {
        Socket socket = null;
        try {
            InetAddress address = InetAddress.getLocalHost();
            socket = new Socket(address, port);
            PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
            System.out.println(String.format("controller sending to port %d: %s", port, message));
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

class DstoreFile{
    private String file_name;
    private String status;

    public DstoreFile(String file_name){
        this.file_name = file_name;
    }

    public String getName() {
        return file_name;
    }

    public void setName(String file_name) {
        this.file_name = file_name;
    }

    public String getStatus() {
        return status;
    }

    public void setStatus(String status) {
        this.status = status;
    }
}

class Index {
    /**
     * Index class, data structure for controller to keep track of stores files.
     */

    // list of pairs of dstore ports and files in the associated dstore
    HashMap<Integer, ArrayList<DstoreFile>> pairs;

    public Index() {
        this.pairs = new HashMap<>();
    }

    public void addFile(Integer dstore_port, String file_name) {
        try {
            for (Integer dstore : pairs.keySet()) {
                // for int dstores
                if (dstore.equals(dstore_port)) {
                    // if dstores == param, add to the corresponding arraylist
                    ArrayList<DstoreFile> dstoreFiles = pairs.get(dstore);
                    dstoreFiles.add(new DstoreFile(file_name));
                }
            }
        } catch (Exception e) {
            System.err.println("error: " + e);
        }
    }

    public void removeFile(Integer dstore_port, String file_name) {
        try {
            for (Integer dstore : pairs.keySet()) {
                // for int dstores
                if (dstore.equals(dstore_port)) {
                    // if dstores == param, remove from corresponding arraylist
                    ArrayList<DstoreFile> dstoreFiles = pairs.get(dstore);
                    for (DstoreFile dstoreFile : dstoreFiles) {
                        if (dstoreFile.getName().equals(file_name)) {
                            dstoreFiles.remove(dstoreFile);
                        }
                    }
                }
            }
        } catch (Exception e) {
            System.err.println("error: " + e);
        }
    }

    public void removeFileFromAll(String file_name) {
        try {
            for (Integer dstore : pairs.keySet()) {
                // for int dstores
                ArrayList<DstoreFile> dstoreFiles = pairs.get(dstore);
                for (DstoreFile dstoreFile : dstoreFiles) {
                    if (dstoreFile.getName().equals(file_name)) {
                        dstoreFiles.remove(dstoreFile);
                    }
                }
            }
        } catch (Exception e) {
            System.err.println("error: " + e);
        }
    }

    public void setAllFileStatus(String file_name, String file_status) {
        try {
            for (Integer dstore : pairs.keySet()) {
                // for int dstore
                ArrayList<DstoreFile> dstoreFiles = pairs.get(dstore);
                for (DstoreFile dstoreFile : dstoreFiles) {
                    if (dstoreFile.getName().equals(file_name)) {
                        // if file == param, set status
                        dstoreFile.setStatus(file_status);
                    }
                }
            }
        } catch (Exception e) {
            System.err.println("error: " + e);
        }
    }

    public void setFileStatus(Integer dstore_port, String file_name, String file_status) {
        try {
            for (Integer dstore : pairs.keySet()) {
                // for int dstore
                if (dstore_port.equals(dstore)) {
                    // if dstore == param
                    ArrayList<DstoreFile> dstoreFiles = pairs.get(dstore);
                    for (DstoreFile dstoreFile : dstoreFiles) {
                        // for File file
                        if (dstoreFile.getName().equals(file_name)) {
                            // if file == param, set status
                            dstoreFile.setStatus(file_status);
                        }
                    }
                }
            }
        } catch (Exception e) {
            System.err.println("error: " + e);
        }
    }

    public void addDstore(Integer dstore_port){
        try {
            pairs.put(dstore_port, new ArrayList<DstoreFile>());
        } catch (Exception e) {
            // duplicate entry should be handled
            System.err.println("error: " + e);
        }
    }

    public void removeDstore(Integer dstore_port){
        try {
            for (Integer dstore : pairs.keySet()) {
                // loop through all pairs and remove dstore
                if (dstore.equals(dstore_port)) {
                    pairs.remove(dstore);
                }
            }
        } catch (Exception e){
            System.err.println("error: " + e);
        }
    }

    public ArrayList<Integer> listDstores() {
        ArrayList<Integer> dstores = new ArrayList<>(pairs.keySet());

        return dstores;
    }

    public ArrayList<String> listFileNames() {
        ArrayList<String> files = new ArrayList<>();

        for (Integer dstore : pairs.keySet()) {
            for (DstoreFile dstoreFile : pairs.get(dstore)) {
                String dStore_filename = dstoreFile.getName();
                if (!files.contains(dStore_filename)) {
                    files.add(dStore_filename);
                }
            }
        }

        return files;
    }
}

public class Controller {
    private int cport; // controller port which it's listening to
    private int R; // replication factor of files
    private int timeout; // timeout duration in seconds
    private int rebalance_period; // rebalance period in seconds

    private Index index;

    public ControllerReceiver receiver;

    public boolean enoughDstores = false;

    public boolean storeInProgress = false;
    public boolean removeInProgress = false;

    public int remainingStoreAcks = 0;
    public int remainingRemoveAcks = 0;


    public static void main(String[] args) {
        Controller controller = new Controller(args);
    }

    public Controller(String[] args) {
        // construct controller object from args
        this.cport = Integer.parseInt(args[0]);
        this.R = Integer.parseInt(args[1]);
        this.timeout = Integer.parseInt(args[2]);
        this.rebalance_period = Integer.parseInt(args[3]);

        this.index = new Index();

        System.out.println(String.format("controller %d: initializing new Controller", cport));
        System.out.println(String.format("controller %d: set replication_factor to %d", cport, R));
        System.out.println(String.format("controller %d: set timeout to %d", cport, timeout));
        System.out.println(String.format("controller %d: set rebalance_period to %d", cport, rebalance_period));

        receiver = new ControllerReceiver(this);
        receiver.start();
    }

    private void rebalance() {
        // rebalance Dstores
    }

    public int getCport() {
        return cport;
    }

    public int getR() {
        return R;
    }

    public int getTimeout() {
        return timeout;
    }

    public int getRebalance_period() {
        return rebalance_period;
    }

    public Index getIndex() {
        return index;
    }
}