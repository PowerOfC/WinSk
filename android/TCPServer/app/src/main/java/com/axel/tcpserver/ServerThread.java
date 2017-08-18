package com.axel.tcpserver;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.net.Socket;
import java.net.SocketException;

/**
 * Created by dev on 18/08/17.
 */

public class ServerThread extends Thread {

    // attr.
    private Client c;

    // constr.
    public ServerThread(Client c) {
        this.c = c;

        // Start up the thread
        start();
    }

    // This runs in a separate thread when start() is called in the
    // constructor.
    public void run() {
        try {
            //TCPCommunicator.progressDialog.dismiss();
            //clientIsOnline();

            // receive a message
            int charsRead = 0;
            char[] buffer = new char[TCPCommunicator.BUFFER_SIZE];
            while ((charsRead = c.in.read(buffer)) != -1) {
                String message = new String(buffer).substring(0, charsRead);
                TCPCommunicator.appendToLog(TCPCommunicator.getTitleWithClientName(TCPCommunicator.RecvTitle, c), message);
            }

            c.s.close();
            c.out.close();
            c.in.close();

            TCPCommunicator.clientIsOffline(c);
            //TCPCommunicator.progressDialog.show(); // this stop the app, do not use

        } catch (IOException e) {
            e.printStackTrace();
            //TCPCommunicator.appendToLog(TCPCommunicator.InformationTitle, "Thread off");
            //TCPCommunicator.appendToLog(TCPCommunicator.ExceptionTitle, e.getMessage());
        }
    }
}
