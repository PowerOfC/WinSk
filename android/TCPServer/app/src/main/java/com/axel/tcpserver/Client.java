package com.axel.tcpserver;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.OutputStream;
import java.net.Socket;

/**
 * Created by dev on 18/08/17.
 */

public class Client {

    // attr.
    public Socket s;
    public BufferedReader in;
    public BufferedWriter out;
    public OutputStream outputStream;
    private int id;
    private String name;
    private String ip;
    public ServerThread thread;

    // constr.
    public Client(int id, Socket s, BufferedReader in, BufferedWriter out, OutputStream outputStream) {
        this.id = id;
        this.s = s;
        this.in = in;
        this.out = out;
        this.outputStream = outputStream;
        this.name = "unknown";
    }

    // getID() : Getter
    public int getID() {
        return id;
    }

    // getName() : Getter
    public String getName() {
        return name;
    }

    // setName() : Setter
    public void setName(String name) {
        this.name = name;
    }

    // getIP() : Getter
    public String getIP() {
        return ip;
    }

    // setIP() : Setter
    public void setIP(String ip) {
        this.ip = ip;
    }
}
