package com.axel.tcpserver;

import android.app.ActionBar;
import android.app.Activity;
import android.app.AlertDialog;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.EditText;

public class SettingsActivity  extends Activity {

    // attr.
    private EditText portEditText;

    // onCreate()
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_settings);

        // get action bar
        ActionBar actionBar = getActionBar();

        // Enabling Up / Back navigation
        actionBar.setDisplayHomeAsUpEnabled(true);

        // Get Controls
        portEditText = (EditText) findViewById(R.id.portEditText);

        portEditText.setText(String.valueOf(MainActivity.SERVER_PORT));
    }

    // onDestroy()
    @Override
    protected void onDestroy() {
        super.onDestroy();
    }

    // applySettingsButtonClicked()
    public void applySettingsButtonClicked(View view)
    {
        try {

            if (portEditText.getText().length() == 0) {
                errorDialog("Port is empty !");
                return;
            }

            // get port number
            int port = Integer.valueOf(portEditText.getText().toString());

            // if it's correct
            if (port >= 0 && port <= 65535) {
                // set new port
                MainActivity.SERVER_PORT = port;
                // close activity
                MainActivity.settingsChanged = true;
                this.finish();
            } else {
                errorDialog("Port must be between 0 and 65535 !");
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    // errorDialog()
    public void errorDialog(String message)
    {
        AlertDialog.Builder alert = new AlertDialog.Builder(this);
        alert.setTitle("Error");
        alert.setMessage(message);
        alert.setPositiveButton("OK", null);
        alert.show();
    }
}