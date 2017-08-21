package com.axel.tcpserver;

import android.app.Activity;
//import android.support.v7.app.AppCompatActivity;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.app.AlertDialog;
import android.text.util.Linkify;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.ListView;
import android.widget.NumberPicker;
import android.widget.ProgressBar;
import android.widget.ScrollView;
import android.widget.TabHost;
import android.os.Handler;
import android.view.View;
import android.widget.EditText;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;


public class MainActivity extends Activity implements OnTCPMessageRecievedListener {

    // attr. / const.
    private final static Boolean TEST_VERSION = false;
    private final static String TURN_OFF_CMD = TEST_VERSION ? "Shell_Exec notepad" : "Turn_Off";
    public static int SERVER_PORT = TEST_VERSION ? 50995 : 50999;
    public static boolean settingsChanged = false;
    private Boolean POPULATE_TODO_LIST = false, REFRESH_TODO_LIST = false;

    // controls
    private TabHost tabHost;
    private Handler handler = new Handler();
    private EditText cmdLine;
    private TextView logView;
    private ScrollView logScrollView;
    private NumberPicker turnOffNumberPicker;
    //private String numberPickerValues[] = new String[] { "0", "5", "10", "15", "30", "45", "60", "90", "120", "150", "180", "210", "240", "300", "360" };
    private ListView todoListView;
    private ListViewAdapter todoListAdapter = new ListViewAdapter(R.layout.listview_row);
    private ListView clientListView;
    private ListViewAdapter clientListAdapter;// = new ListViewAdapter(R.layout.clientlistview_row);
    private ArrayList<HashMap<String, String>> clientList = new ArrayList<HashMap<String, String>>();
    private ProgressBar loadingProgressbar;
    private TextView loadingTextView;
    private Button refreshToDoBtn;
    private CheckBox sendToAllCheckBox;

    // onCreate()
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Get Controls
        cmdLine = (EditText) findViewById(R.id.cmdLine);
        logView = (TextView) findViewById(R.id.logView);
        logScrollView = (ScrollView) findViewById(R.id.logScrollView);
        turnOffNumberPicker = (NumberPicker) findViewById(R.id.turnOffNumberPicker);
        turnOffNumberPicker.setMinValue(0);
        turnOffNumberPicker.setMaxValue(600);
        //turnOffNumberPicker.setMaxValue(numberPickerValues.length - 1);
        //turnOffNumberPicker.setDisplayedValues(numberPickerValues);
        todoListView = (ListView) findViewById(R.id.ToDoListView);
        clientListView = (ListView) findViewById(R.id.ClientListView);
        loadingProgressbar = (ProgressBar) findViewById(R.id.loadingProgressBar);
        loadingTextView = (TextView) findViewById(R.id.loadingTextView);
        refreshToDoBtn = (Button) findViewById(R.id.refreshToDoBtn);
        sendToAllCheckBox = (CheckBox) findViewById(R.id.sendToAllCheckBox);
        TabHost host = (TabHost) findViewById(R.id.tabHost);
        host.setup();

        //Tab 1
        TabHost.TabSpec spec = host.newTabSpec("TurnOffTab");
        spec.setContent(R.id.tab1);
        spec.setIndicator("", getResources().getDrawable(R.drawable.ic_turn_off));
        host.addTab(spec);

        //Tab 2
        spec = host.newTabSpec("LogTab");
        spec.setContent(R.id.tab2);
        spec.setIndicator("", getResources().getDrawable(R.drawable.ic_log));
        host.addTab(spec);

        //Tab 3
        spec = host.newTabSpec("ClientTab");
        spec.setContent(R.id.tab3);
        spec.setIndicator("", getResources().getDrawable(R.drawable.ic_client));
        host.addTab(spec);

        //Tab 4
        spec = host.newTabSpec("ToDoTab");
        spec.setContent(R.id.tab4);
        spec.setIndicator("", getResources().getDrawable(R.drawable.ic_todo));
        host.addTab(spec);

        host.setOnTabChangedListener(new TabHost.OnTabChangeListener(){
            @Override
            public void onTabChanged(String tabId) {
                if(tabId.equals("LogTab")) {
                    // scroll logView
                    logScrollView.post(new Runnable() {
                        public void run() {
                            logScrollView.fullScroll(View.FOCUS_DOWN);
                        }
                    });
                }
                else if(tabId.equals("ToDoTab")) {
                    // if a client is connected
                    if (clientListAdapter.selectedItemPosition != -1) {
                        // get ToDoList
                        refreshToDoBtn.performClick();
                    }
                }
            }
        });

        // setup clientList
        clientListAdapter = new ListViewAdapter(this, R.layout.clientlistview_row, clientList);
        clientListView.setAdapter(clientListAdapter);

        // Start Server
        appendToLogView("Starting Server...\n" +
                        "Server IP: " + TCPCommunicator.getIpAddress() +
                        "Server Port: " + SERVER_PORT, "");
        TCPCommunicator writer = TCPCommunicator.getInstance(true);
        TCPCommunicator.addListener(this);
        writer.init(this, SERVER_PORT);
        loadingTextView.setText("Waiting for a client...");
    }

    // onDestroy()
    @Override
    protected void onDestroy() {
        TCPCommunicator.closeStreams();
        super.onDestroy();
    }

    // onResume()
    @Override
    protected void onResume() {
        super.onResume();
        if (settingsChanged) {
            settingsChanged = false;
            this.recreate();
        }
    }

    // onCreateOptionsMenu()
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        MenuInflater inflater = getMenuInflater();
        inflater.inflate(R.menu.activity_main_menu, menu);

        return super.onCreateOptionsMenu(menu);
    }

    /**
     * On selecting action bar icons
     * */
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Take appropriate action for each action item click
        switch (item.getItemId()) {
            case R.id.action_settings:
                // settings
                showSettings();
                return true;
            case R.id.action_about:
                // about
                showAbout();
                return true;
            default:
                return super.onOptionsItemSelected(item);
        }
    }

    // showSettings()
    private void showSettings()
    {
        Intent i = new Intent(MainActivity.this, SettingsActivity.class);
        startActivity(i);
    }

    // showAbout()
    private void showAbout()
    {
        AlertDialog.Builder aboutWindow = new AlertDialog.Builder(this);
        final String email = "axel.prog.25@gmail.com";
        final String AboutDialogMessage = "TCP Server\n\nVersion " + versionName() + "\n\nBy AXeL\n\nContact: ";
        final TextView tx = new TextView(this); //we create a textview to store the dialog text/contents
        tx.setText(AboutDialogMessage + email); //we set the text/contents
        tx.setAutoLinkMask(RESULT_OK); //to linkify any website or email links
        tx.setTextSize(15); //setting the text size
        tx.setPadding(20, 20, 20, 20); // set padding
        Linkify.addLinks(tx, Linkify.ALL); // linkify the text

        aboutWindow.setIcon(R.mipmap.ic_launcher); //to show the icon next to the title "About"
        aboutWindow.setTitle("About"); //set the title of the about box to say "About"
        aboutWindow.setView(tx); //set the textview on the dialog box
        aboutWindow.setPositiveButton("Close", null);
        aboutWindow.show();
    }

    // versionName()
    public String versionName() {
        try {
            PackageInfo pInfo = getPackageManager().getPackageInfo(getPackageName(), 0);
            return pInfo.versionName;
        } catch (PackageManager.NameNotFoundException e) {
            return "Unknown";
        }
    }

    // populateToDoList()
    private void populateToDoList(String text) {

        ArrayList<HashMap<String, String>> list = new ArrayList<HashMap<String, String>>();

        // if not empty => proceed
        if (text.length() > 0 && ! text.equals("[No ToDo]") && ! text.equals("null")) {

            String id, cmd, time;
            char c;
            Boolean GET_ID, GET_CMD, GET_TIME;

            // initialisation
            id = cmd = time = "";
            GET_ID = GET_CMD = GET_TIME = false;

            // parcours de 'text'
            for (int i = 0; i < text.length(); i++) {
                c = text.charAt(i);

                if (c == '[' || i == text.length() - 1) {
                    if (GET_CMD) {
                        if (i == text.length() - 1) cmd += c;
                        HashMap<String, String> hashmap = new HashMap<String, String>();
                        hashmap.put(ListViewAdapter.FIRST_COLUMN, id);
                        hashmap.put(ListViewAdapter.SECOND_COLUMN, cmd);
                        hashmap.put(ListViewAdapter.THIRD_COLUMN, time + " min.");
                        list.add(hashmap);
                        id = cmd = time = ""; // rénitialisation
                        GET_CMD = false;
                    }
                    GET_ID = true;
                } else if (c == ']') {
                    GET_ID = false;
                    GET_TIME = true;
                } else if (c == '|') {
                    GET_TIME = false;
                    GET_CMD = true;
                } else {
                    if (GET_ID) id += c;
                    else if (GET_TIME) time += c;
                    else cmd += c;
                }
            }
        }

        todoListAdapter = new ListViewAdapter(this, R.layout.listview_row, list);
        todoListView.setAdapter(todoListAdapter);
    }

    // addToClientList()
    private void addToClientList(int clientID, String clientName, String clientIP) {

        // if not empty => proceed
        //if (clientName.length() > 0) {

            /*
            String userName, machineName;

            String[] splitedName = clientName.split("-");
            userName = splitedName[0];
            machineName = splitedName[1];
            */

            HashMap<String, String> hashmap = new HashMap<String, String>();
            hashmap.put("CLIENT_ID", Integer.toString(clientID));
            hashmap.put(ListViewAdapter.FIRST_COLUMN, clientName);
            hashmap.put(ListViewAdapter.SECOND_COLUMN, clientIP);
            hashmap.put(ListViewAdapter.THIRD_COLUMN, getCurrentTime());
            clientList.add(hashmap);

            //Log.e("selected", String.valueOf(clientListAdapter.selectedItemPosition));
            if (clientListAdapter.selectedItemPosition == -1) {
                clientListAdapter.selectedItemPosition = clientList.size() - 1;
                clientListAdapter.forceSelect = true;
            }

            clientListAdapter.notifyDataSetChanged();
        //}
    }

    // removeFromClientList()
    private void removeFromClientList(int clientID) {
        for (HashMap<String, String> client : clientList) {
            if (Integer.valueOf(client.get("CLIENT_ID")) == clientID) {
                clientList.remove(client);

                clientListAdapter.setSelected(null);
                if (clientList.size() > 0) {
                    clientListAdapter.selectedItemPosition = clientList.size() - 1;
                    clientListAdapter.forceSelect = true;
                }
                else {
                    clientListAdapter.selectedItemPosition = -1;
                }

                clientListAdapter.notifyDataSetChanged();
                break;
            }
        }
    }

    // disconnectButtonClicked()
    public void disconnectButtonClicked(View view) {
        try {

            if (clientListAdapter.selectedItemPosition != -1) {
                final View finalView = view;
                DialogInterface.OnClickListener dialogClickListener = new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        switch (which) {
                            case DialogInterface.BUTTON_POSITIVE:
                                //Yes button clicked
                                Thread thread = new Thread(new Runnable() {

                                    @Override
                                    public void run() {
                                        String id = clientListAdapter.getSubItem(clientListAdapter.selectedItemPosition, "CLIENT_ID").toString();
                                        TCPCommunicator.disconnectClient(Integer.valueOf(id));
                                    }
                                });
                                thread.start();
                                break;

                            case DialogInterface.BUTTON_NEGATIVE:
                                //No button clicked
                                break;
                        }
                    }
                };

                AlertDialog.Builder builder = new AlertDialog.Builder(finalView.getContext());
                builder.setMessage("Disconnect Client: " + clientListAdapter.getSubItem(clientListAdapter.selectedItemPosition, "ID") + " ?");
                builder.setPositiveButton("Yes", dialogClickListener);
                builder.setNegativeButton("No", dialogClickListener);
                builder.show();
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    // sendCommand()
    private void sendCommand(String cmd, boolean sendToAll) {
        try {
            if (TCPCommunicator.isSomeoneConnected()) {
                if (cmd.length() > 0) {
                    int clientID = clientListAdapter.selectedItemPosition != -1 ? Integer.valueOf(clientListAdapter.getSubItem(clientListAdapter.selectedItemPosition, "CLIENT_ID").toString()) : 0;
                    //Log.e("clientID", String.valueOf(clientID));
                    TCPCommunicator.writeToSocket(cmd, clientID, sendToAll);
                }
                //else {
                    //TCPCommunicator.appendToLog(TCPCommunicator.InformationTitle, "Empty command!");
                //}
            }
            //else {
                //TCPCommunicator.appendToLog(TCPCommunicator.InformationTitle, "No client connected!");
            //}
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    // sendButtonClicked()
    public void sendButtonClicked(View view) {
        try {
            Thread thread = new Thread(new Runnable() {

                @Override
                public void run() {
                    boolean sendToAll = sendToAllCheckBox.isChecked();
                    sendCommand(cmdLine.getText().toString(), sendToAll);
                }
            });
            thread.start();

        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    // clearButtonClicked()
    public void clearButtonClicked(View view) {
        try {
            DialogInterface.OnClickListener dialogClickListener = new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    switch (which) {
                        case DialogInterface.BUTTON_NEGATIVE:
                            // CmdLine button clicked
                            cmdLine.setText("");
                            break;

                        case DialogInterface.BUTTON_NEUTRAL:
                            // Log button clicked
                            logView.setText("");
                            break;

                        case DialogInterface.BUTTON_POSITIVE:
                            // Cancel button clicked
                            break;
                    }
                }
            };

            AlertDialog.Builder builder = new AlertDialog.Builder(this);
            builder.setMessage("Confirm Clear ?");
            builder.setNegativeButton("CmdLine", dialogClickListener);
            builder.setNeutralButton("Log", dialogClickListener);
            builder.setPositiveButton("Cancel", dialogClickListener);
            builder.show();
        } catch (Exception e) {
            e.printStackTrace();
            //Log.e("exception", e.getMessage());
        }
    }

    // turnOffButtonClicked()
    public void turnOffButtonClicked(View view) {
        try {
            Thread thread = new Thread(new Runnable() {

                @Override
                public void run() {
                    int time = turnOffNumberPicker.getValue();
                    String cmd = "";

                    if (time == 0)
                        cmd = TURN_OFF_CMD;
                    else
                        cmd = "Add_ToDo " + time + "|" + TURN_OFF_CMD;

                    sendCommand(cmd, false);
                }
            });
            thread.start();

        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    // refreshToDoButtonClicked()
    public void refreshToDoButtonClicked(View view) {
        try {
            Thread thread = new Thread(new Runnable() {

                @Override
                public void run() {
                    sendCommand("List_ToDo", false);
                    POPULATE_TODO_LIST = true;
                }
            });
            thread.start();
        } catch (Exception e) {
            e.printStackTrace();
            //Log.e("exception", e.getMessage());
        }
    }

    // deleteToDoButtonClicked()
    public void deleteToDoButtonClicked(View view) {
        try {

            if (todoListAdapter.selectedItemPosition != -1) {
                final View finalView = view;
                DialogInterface.OnClickListener dialogClickListener = new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        switch (which) {
                            case DialogInterface.BUTTON_POSITIVE:
                                //Yes button clicked
                                Thread thread = new Thread(new Runnable() {

                                    @Override
                                    public void run() {
                                        String id = todoListAdapter.getSubItem(todoListAdapter.selectedItemPosition, "ID").toString();
                                        sendCommand("Delete_ToDo " + id, false);
                                        //askToRefreshToDoList(finalView);
                                        REFRESH_TODO_LIST = true;
                                    }
                                });
                                thread.start();
                                break;

                            case DialogInterface.BUTTON_NEGATIVE:
                                //No button clicked
                                break;
                        }
                    }
                };

                AlertDialog.Builder builder = new AlertDialog.Builder(finalView.getContext());
                builder.setMessage("Delete Item " + todoListAdapter.getItem(todoListAdapter.selectedItemPosition) + " ?");
                builder.setPositiveButton("Yes", dialogClickListener);
                builder.setNegativeButton("No", dialogClickListener);
                builder.show();
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    // addToDoButtonClicked()
    public void addToDoButtonClicked(View view) {
        try {
            // set up addToDo Dialog
            AlertDialog.Builder dialogBuilder = new AlertDialog.Builder(this);
            LayoutInflater inflater = this.getLayoutInflater();
            final View dialogView = inflater.inflate(R.layout.add_todo_dialog, null);
            dialogBuilder.setView(dialogView);

            final EditText time = (EditText) dialogView.findViewById(R.id.timeEditText);
            final EditText cmd = (EditText) dialogView.findViewById(R.id.cmdEditText);
            final View finalView = view;

            dialogBuilder.setTitle("Add ToDo");
            dialogBuilder.setPositiveButton("Add", new DialogInterface.OnClickListener() {
                public void onClick(DialogInterface dialog, int whichButton) {
                    Thread thread = new Thread(new Runnable() {

                        @Override
                        public void run() {
                            sendCommand("Add_ToDo " + time.getText() + "|" + cmd.getText(), false);
                            //askToRefreshToDoList(finalView);
                            REFRESH_TODO_LIST = true;
                        }
                    });
                    thread.start();
                }
            });
            dialogBuilder.setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
                public void onClick(DialogInterface dialog, int whichButton) {
                    // dismiss
                }
            });
            AlertDialog builder = dialogBuilder.create();
            builder.show();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    // freeToDoButtonClicked()
    public void freeToDoButtonClicked(View view) {
        try {
            final View finalView = view;

            DialogInterface.OnClickListener dialogClickListener = new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    switch (which) {
                        case DialogInterface.BUTTON_POSITIVE:
                            //Yes button clicked
                            Thread thread = new Thread(new Runnable() {

                                @Override
                                public void run() {
                                    sendCommand("Free_ToDo", false);
                                    //askToRefreshToDoList(finalView);
                                    REFRESH_TODO_LIST = true;
                                }
                            });
                            thread.start();
                            break;

                        case DialogInterface.BUTTON_NEGATIVE:
                            //No button clicked
                            break;
                    }
                }
            };

            AlertDialog.Builder builder = new AlertDialog.Builder(finalView.getContext());
            builder.setMessage("Free List ?");
            builder.setPositiveButton("Yes", dialogClickListener);
            builder.setNegativeButton("No", dialogClickListener);
            builder.show();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    // askToRefreshToDoList()
    private void askToRefreshToDoList(View view) {
        final View finalView = view;
        handler.post(new Runnable() {

            @Override
            public void run() {
                try {
                    DialogInterface.OnClickListener dialogClickListener = new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            switch (which) {
                                case DialogInterface.BUTTON_POSITIVE:
                                    //Yes button clicked
                                    refreshToDoButtonClicked(finalView);
                                    break;

                                case DialogInterface.BUTTON_NEGATIVE:
                                    //No button clicked
                                    break;
                            }
                        }
                    };

                    AlertDialog.Builder builder = new AlertDialog.Builder(finalView.getContext());
                    builder.setMessage("Refresh List ?");
                    builder.setPositiveButton("Yes", dialogClickListener);
                    builder.setNegativeButton("No", dialogClickListener);
                    builder.show();
                } catch (Exception e) {
                    e.printStackTrace();
                    //Log.e("exception", e.getMessage());
                }
            }
        });
    }

    // onTCPMessageRecieved()
    @Override
    public void onTCPMessageRecieved(String message, String title) {
        final String msg = message;
        final String finalTitle = title;
        handler.post(new Runnable() {

            @Override
            public void run() {
                try {
                    appendToLogView(msg, finalTitle);

                    // if it's a recv message
                    if (finalTitle.contains(TCPCommunicator.RecvTitle)) {
                        // if asked to populate ToDoList + we have receive the data
                        if (POPULATE_TODO_LIST) {
                            populateToDoList(msg);
                            POPULATE_TODO_LIST = false;
                        }
                        // or asked to refresh ToDoList
                        else if (REFRESH_TODO_LIST)
                        {
                            refreshToDoBtn.performClick();
                            REFRESH_TODO_LIST = false;
                        }
                    }
                    // or even an error/exception occured
                    else if (finalTitle.equals(TCPCommunicator.ExceptionTitle))
                    {
                        POPULATE_TODO_LIST = false;
                        REFRESH_TODO_LIST = false;
                    }

                } catch (Exception e) {
                    e.printStackTrace();
                    //Log.e("exception", e.getMessage());
                }
            }
        });
    }

    // onClientConnection()
    @Override
    public void onClientConnection(int id, String name, String ip, int count) {
        final String clientFullName = name;
        final String clientIP = ip;
        final int clientsCount = count;
        final int clientID = id;

        handler.post(new Runnable() {

            @Override
            public void run() {
                try {
                    if (clientsCount > 0) {
                        // hide loading Progress Bar
                        loadingProgressbar.setVisibility(View.INVISIBLE);
                        // change loading text & set icon
                        loadingTextView.setText(clientsCount + " Client(s) Connected!");
                        loadingTextView.setCompoundDrawablesWithIntrinsicBounds(R.drawable.ic_ok, 0, 0, 0);
                        // add to clientList
                        addToClientList(clientID, clientFullName, clientIP);
                    }
                } catch (Exception e) {
                    e.printStackTrace();
                    //Log.e("exception", e.getMessage());
                }
            }
        });
    }

    // onClientDeconnection()
    @Override
    public void onClientDeconnection(int id, int count) {
        final int clientsCount = count;
        final int clientID = id;
        handler.post(new Runnable() {

            @Override
            public void run() {
                try {
                    if (clientsCount == 0) {
                        // show loading Progress Bar
                        loadingProgressbar.setVisibility(View.VISIBLE);
                        // change loading text & remove icon
                        loadingTextView.setText("Waiting for a client...");
                        loadingTextView.setCompoundDrawablesWithIntrinsicBounds(0, 0, 0, 0);
                    }
                    else {
                        loadingTextView.setText(clientsCount + " Client(s) Connected!");
                    }
                    // remove from clientList
                    removeFromClientList(clientID);
                } catch (Exception e) {
                    e.printStackTrace();
                    //Log.e("exception", e.getMessage());
                }
            }
        });
    }

    // appendToLogView()
    private void appendToLogView(String text, String title) {
        logView.append("[" + getCurrentTime() + "] " + title + text + "\n");

        logScrollView.post(new Runnable() {
            public void run() {
                logScrollView.fullScroll(View.FOCUS_DOWN);
            }
        });
    }

    /**
     * getCurrentTime() it will return system time
     *
     * @return
     */
    public static String getCurrentTime() {
        Date dt = new Date();
        int hours = dt.getHours();
        int minutes = dt.getMinutes();
        int seconds = dt.getSeconds();
        String curTime = hours + ":" + minutes + ":" + seconds;

        return curTime;
    }// end of getCurrentTime()
}
