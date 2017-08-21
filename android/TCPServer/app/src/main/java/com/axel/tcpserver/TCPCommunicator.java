package com.axel.tcpserver;

/**
 * Created by dev on 16/07/17.
 */

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketException;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.List;

import android.app.Activity;
import android.app.ProgressDialog;
import android.os.AsyncTask;
import android.os.Handler;
import android.util.Log;
import android.app.ProgressDialog;

public class TCPCommunicator {
    private static TCPCommunicator uniqInstance;
    private static int serverPort;
    private static List<OnTCPMessageRecievedListener> allListeners;
    private static ServerSocket ss;
    private static List<Client> clients;
    private static Handler handler = new Handler();
    //private static ProgressDialog progressDialog;
    private static int currentID = 0;

    public static final int BUFFER_SIZE = 1024;
    public static boolean exit = false;

    public static final String SendTitle = "send: ";
    public static final String RecvTitle = "recv: ";
    public static final String ExceptionTitle = "exception: ";
    public static final String InformationTitle = "info: ";
    public static final String NewClientTitle = "new client: ";

    // Enum
    public enum TCPWriterErrors {
        UnknownHostException, IOException, OtherProblem, OK
    }

    // Constr.
    private TCPCommunicator() {
        allListeners = new ArrayList<OnTCPMessageRecievedListener>();
        clients = new ArrayList<Client>();
    }

    // getInstance()
    public static TCPCommunicator getInstance(boolean clear) {
        if (uniqInstance == null) {
            uniqInstance = new TCPCommunicator();
        }
        else if (clear) {
            allListeners.clear();
            clients.clear();
        }
        return uniqInstance;
    }

    // init()
    public TCPWriterErrors init(Activity obj, int port) {
        exit = false; // rénitialisation
        setServerPort(port);
        InitTCPServerTask task = new InitTCPServerTask();
        task.execute(new Void[0]);
        // Show Progress Dialog
        /*
        progressDialog = new ProgressDialog(obj);
        progressDialog.setTitle("Server ON");
        progressDialog.setMessage("Waiting for a client...");
        progressDialog.setIndeterminate(true);
        progressDialog.show();
        */
        return TCPWriterErrors.OK;
    }

    // writeSocket()
    public static TCPWriterErrors writeToSocket(String msg, int clientID, boolean sendToAll) {
        try {
            // check all connected clients
            for (Client c : clients) {
                if (sendToAll || clientID == c.getID()) { // if client founded or send to all
                    appendToLog(getTitleWithClientName(SendTitle, c), msg);
                    c.out.write(msg);
                    c.out.flush();
                    if (! sendToAll) break;
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
            appendToLog(ExceptionTitle, e.getMessage());
        }
        return TCPWriterErrors.OK;

    }

    // addListner()
    public static void addListener(OnTCPMessageRecievedListener listener) {
        allListeners.add(listener);
    }

    // getServerPort() : Getter
    public static int getServerPort() {
        return serverPort;
    }

    // setServerPort() : Setter
    public static void setServerPort(int serverPort) {
        TCPCommunicator.serverPort = serverPort;
    }

    // InitTCPServerTask class
    public class InitTCPServerTask extends AsyncTask<Void, Void, Void> {
        // Constr.
        public InitTCPServerTask() {
            // ...
        }

        // doInBackground()
        @Override
        protected Void doInBackground(Void... params) {


            try {
                ss = new ServerSocket(TCPCommunicator.getServerPort());

                while (!exit) {
                    Socket s = ss.accept();

                    BufferedReader in = new BufferedReader(new InputStreamReader(s.getInputStream()));
                    OutputStream outputStream = s.getOutputStream();
                    BufferedWriter out = new BufferedWriter(new OutputStreamWriter(outputStream));

                    // get client name & ip & id
                    char[] buffer = new char[BUFFER_SIZE];
                    int charsRead = in.read(buffer);
                    String name = new String(buffer).substring(0, charsRead);
                    String ip = s.getInetAddress().getHostName();
                    int id = newID();

                    // create client instance
                    Client c = new Client(id, s, in, out, outputStream);
                    c.setName(name);
                    c.setIP(ip);

                    // add client
                    clients.add(c);
                    clientIsOnline(id, name, ip);

                    // run server thread (per client)
                    new ServerThread(c);
                }

                for (Client c : clients)
                    c.s.close();

            } catch (IOException e) {
                e.printStackTrace();
                appendToLog(ExceptionTitle, e.getMessage());
            }
            return null;

        }

    }

    // newID()
    private int newID() {
        return ++currentID;
    }

    // appendToLog()
    public static void appendToLog(String title, String msg) {
        final String finalTitle = title;
        final String finalMessage = msg;
        handler.post(new Runnable() {

            @Override
            public void run() {
                // TODO Auto-generated method stub
                for (OnTCPMessageRecievedListener listener : allListeners)
                    listener.onTCPMessageRecieved(finalMessage, finalTitle);
                //Log.e("TCP Server", finalMessage);
            }
        });
    }

    // getTitleWithClientName()
    public static String getTitleWithClientName(String title, Client c) {
        return title + "<" + c.getName() + "> ";
    }

    // isSomeoneConnected()
    public static boolean isSomeoneConnected() {
        return clients.size() > 0 ? true : false;
    }

    // clientIsOnline()
    public void clientIsOnline(int clientID, String clientName, String clientIP) {
        final String name = clientName;
        final String ip = clientIP;
        final int count = clients.size();
        final int id = clientID;

        handler.post(new Runnable() {

            @Override
            public void run() {
                appendToLog(NewClientTitle, name);

                for (OnTCPMessageRecievedListener listener : allListeners)
                    listener.onClientConnection(id, name, ip, count);
            }
        });
    }

    // clientIsOffline()
    public static void clientIsOffline(Client c) {
        final String clientName = c.getName(); // save client name
        final int clientID = c.getID();
        clients.remove(c); // remove client
        final int count = clients.size();

        handler.post(new Runnable() {

            @Override
            public void run() {
                appendToLog(InformationTitle, "<" + clientName + "> Disconnected!");

                for (OnTCPMessageRecievedListener listener : allListeners)
                    listener.onClientDeconnection(clientID, count);
            }
        });
    }

    // disconnectClient()
    public static void disconnectClient(int clientID) {
        try {
            for (Client c : clients) {
                if (c.getID() == clientID) {
                    c.s.close();
                    c.out.close();
                    c.in.close();

                    clientIsOffline(c);
                    break;
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
            appendToLog(ExceptionTitle, e.getMessage());
        }
    }

    // getIpAddress()
    public static String getIpAddress() {
        String ip = "";
        try {
            Enumeration<NetworkInterface> enumNetworkInterfaces = NetworkInterface.getNetworkInterfaces();

            while (enumNetworkInterfaces.hasMoreElements()) {
                NetworkInterface networkInterface = enumNetworkInterfaces.nextElement();
                Enumeration<InetAddress> enumInetAddress = networkInterface.getInetAddresses();

                while (enumInetAddress.hasMoreElements()) {
                    InetAddress inetAddress = enumInetAddress.nextElement();

                    if (inetAddress.isSiteLocalAddress()) {
                        ip += inetAddress.getHostAddress() + "\n";
                    }

                }

            }

        } catch (SocketException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
            ip += "Something Wrong! " + e.toString() + "\n";
        }

        return ip != "" ? ip : "-\n";
    }

    // closeStreams()
    public static void closeStreams() {
        // TODO Auto-generated method stub
        try {
            exit = true;
            ss.close();
            for (Client c : clients) {
                c.s.close();
                c.out.close();
                c.in.close();
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
