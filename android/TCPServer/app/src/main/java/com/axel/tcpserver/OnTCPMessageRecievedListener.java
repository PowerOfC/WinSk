package com.axel.tcpserver;

/**
 * Created by dev on 16/07/17.
 */

public interface OnTCPMessageRecievedListener {
    public void onTCPMessageRecieved(String message, String title);
    public void onClientConnection(int id, String name, String ip, int count);
    public void onClientDeconnection(int id, int count);
}

