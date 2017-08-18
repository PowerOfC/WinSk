package com.axel.tcpserver;

/**
 * Created by dev on 18/07/17.
 */

import java.util.ArrayList;
import java.util.HashMap;


import android.app.Activity;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.RadioButton;
import android.widget.TextView;

public class ListViewAdapter extends BaseAdapter {

    public ArrayList<HashMap<String, String>> list;
    private Activity activity;
    public static final String FIRST_COLUMN = "ID"; // or "client name"
    public static final String SECOND_COLUMN = "Cmd"; // or "client IP"
    public static final String THIRD_COLUMN = "Time"; // or "client connection time"
    private RadioButton selected = null;
    public int selectedItemPosition = -1;
    private int layout;
    public boolean forceSelect = false;

    public ListViewAdapter(int layout) {
        super();
        this.selectedItemPosition = -1;
        this.layout = layout;
    }

    public ListViewAdapter(Activity activity, int layout, ArrayList<HashMap<String, String>> list) {
        super();
        this.activity = activity;
        this.layout = layout;
        this.list = list;
        this.selectedItemPosition = -1;
    }

    public void setSelected(RadioButton sel) {
        this.selected = sel;
    }

    @Override
    public int getCount() {
        // TODO Auto-generated method stub
        return list.size();
    }

    @Override
    public Object getItem(int position) {
        // TODO Auto-generated method stub
        return list.get(position);
    }

    public Object getSubItem(int position, String subItemColumnName) {
        // TODO Auto-generated method stub
        return list.get(position).get(subItemColumnName);
    }

    @Override
    public long getItemId(int position) {
        // TODO Auto-generated method stub
        return 0;
    }

    private class ViewHolder {
        RadioButton radioButton;
        TextView txtFirst;
        TextView txtSecond;
        TextView txtThird;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        // TODO Auto-generated method stub

        final ViewHolder holder;
        final int pos = position;

        LayoutInflater inflater = activity.getLayoutInflater();

        if (convertView == null) {

            convertView = inflater.inflate(layout, null);
            holder = new ViewHolder();

            holder.radioButton = (RadioButton) convertView.findViewById(R.id.radioButton);
            holder.txtFirst = (TextView) convertView.findViewById(R.id.TextFirst);
            holder.txtSecond = (TextView) convertView.findViewById(R.id.TextSecond);
            holder.txtThird = (TextView) convertView.findViewById(R.id.TextThird);

            convertView.setTag(holder);
        } else {

            holder = (ViewHolder) convertView.getTag();
        }

        HashMap<String, String> map = list.get(position);
        holder.txtFirst.setText(map.get(FIRST_COLUMN));
        holder.txtSecond.setText(map.get(SECOND_COLUMN));
        holder.txtThird.setText(map.get(THIRD_COLUMN));
        holder.radioButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                // save selected item position
                selectedItemPosition = pos;

                // checks if we already have a checked radiobutton. If we don't, we can assume that
                // the user clicked the current one to check it, so we can remember it.
                if (selected == null) {
                    selected = (RadioButton) view;
                    selected.setChecked(true);
                }

                // If the user clicks on a RadioButton that we've already stored as being checked, we
                // don't want to do anything.
                if (selected == view)
                    return;

                // Otherwise, uncheck the currently checked RadioButton, check the newly checked
                // RadioButton, and store it for future reference.
                selected.setChecked(false);
                ((RadioButton) view).setChecked(true);
                selected = (RadioButton) view;
            }
        });
        //holder.radioButton.setChecked(false);

        if (forceSelect && selected == null) {
            //Log.e("forceSelect", String.valueOf(pos));

            selected = holder.radioButton;
            selected.setChecked(true);

            forceSelect = false; // reset value to false
        }

        return convertView;
    }

}
