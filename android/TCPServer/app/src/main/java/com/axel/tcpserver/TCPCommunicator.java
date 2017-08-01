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
    private static Socket s;
    private static BufferedReader in;
    private static BufferedWriter out;
    private static OutputStream outputStream;
    private static Handler handler = new Handler();
    private static ProgressDialog progressDialog;

    public static final int BUFFER_SIZE = 1024;
    public static boolean exit = false;
    public static boolean isConnected = false;

    public static final String SendTitle = "send: ";
    public static final String RecvTitle = "recv: ";
    public static final String ExceptionTitle = "exception: ";
    public static final String InformationTitle = "info: ";

    // Enum
    public enum TCPWriterErrors {
        UnknownHostException, IOException, OtherProblem, OK
    }

    // Constr.
    private TCPCommunicator() {
        allListeners = new ArrayList<OnTCPMessageRecievedListener>();
    }

    // getInstance()
    public static TCPCommunicator getInstance() {
        if (uniqInstance == null) {
            uniqInstance = new TCPCommunicator();
        }
        return uniqInstance;
    }

    // init()
    public TCPWriterErrors init(Activity obj, int port) {
        exit = false; // rénitialisation
        isConnected = false;
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
    public static TCPWriterErrors writeToSocket(String msg) {
        try {
            appendToLog(SendTitle, msg);
            out.write(msg);
            out.flush();
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
                    s = ss.accept();

                    //progressDialog.dismiss();
                    clientIsOnline();

                    in = new BufferedReader(new InputStreamReader(s.getInputStream()));
                    outputStream = s.getOutputStream();
                    out = new BufferedWriter(new OutputStreamWriter(outputStream));

                    // receive a message
                    String message = "";
                    int charsRead = 0;
                    char[] buffer = new char[BUFFER_SIZE];
                    while ((charsRead = in.read(buffer)) != -1) {
                        message += new String(buffer).substring(0, charsRead);
                        appendToLog(RecvTitle, message);
                        message = ""; // ready for new message
                    }

                    clientIsOffline();
                    //progressDialog.show(); // this stop the app, do not use
                }

                s.close();

            } catch (IOException e) {
                e.printStackTrace();
                appendToLog(ExceptionTitle, e.getMessage());
            }
            return null;

        }

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

    // clientIsOnline()
    public void clientIsOnline() {
        isConnected = true;

        handler.post(new Runnable() {

            @Override
            public void run() {
                appendToLog(InformationTitle, "Client Connected!");

                for (OnTCPMessageRecievedListener listener : allListeners)
                    listener.onClientConnection();
            }
        });
    }

    // clientIsOffline()
    public void clientIsOffline() {
        isConnected = false;

        handler.post(new Runnable() {

            @Override
            public void run() {
                appendToLog(InformationTitle, "Client Disconnected!");

                for (OnTCPMessageRecievedListener listener : allListeners)
                    listener.onClientDeconnection();
            }
        });
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
            s.close();
            ss.close();
            out.close();
            in.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
