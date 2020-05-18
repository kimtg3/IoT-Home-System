package com.donga.computer.sisa;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.Toast;

import java.net.NetworkInterface;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

public class MacaddActivity extends BluetoothActivity {
    String macAddress = getMACAddress("wlan0");
    //ListView listView;
    //ArrayList<String> midList;
    //ArrayAdapter<String> adapter;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_macadd);

        listView = (ListView)findViewById(R.id.list);
        midList = new ArrayList<String>();
        adapter = new ArrayAdapter<String>(this, android.R.layout.simple_list_item_1, midList);
        listView.setAdapter(adapter);

        listView.setOnItemLongClickListener(new AdapterView.OnItemLongClickListener() {
            public boolean onItemLongClick(AdapterView<?> arg0, View arg1, int arg2, long arg3) {
                System.out.println("=====================" + arg2 + "=====================");
                midList.remove(arg2); //꾹 누르면 삭제
                sendData(String.valueOf(arg2));
                adapter.notifyDataSetChanged();
                Toast.makeText(getApplicationContext(), "MAC 주소가 삭제되었습니다", Toast.LENGTH_SHORT).show();

                return false;
            }
        });

        mod = 2;
    }

    public static String getMACAddress(String interfaceName) {
        try {
            List<NetworkInterface> interfaces = Collections.list(NetworkInterface.getNetworkInterfaces());

            for (NetworkInterface intf : interfaces) {
                if (interfaceName != null) {
                    if (!intf.getName().equalsIgnoreCase(interfaceName)) continue;
                }

                byte[] mac = intf.getHardwareAddress();

                if (mac == null) {
                    return "";
                }

                StringBuilder buf = new StringBuilder();

                for (int idx = 0; idx < mac.length; idx++) {
                    buf.append(String.format("%02X:", mac[idx]));
                }
                if (buf.length() > 0) {
                    buf.deleteCharAt(buf.length() - 1);
                }

                return buf.toString();
            }
        } catch (Exception ex) {
            //Log.e("macAddress", ex.toString());
        } // for now eat exceptions
        return "";
    }

    public void OnClickHandler(View view) {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);

        builder.setTitle("MAC 주소").setMessage(macAddress);
        Log.d("macAddress", macAddress);

        builder.setNegativeButton("취소", new DialogInterface.OnClickListener(){ //취소버튼
            @Override
            public void onClick(DialogInterface dialog, int id)
            {
                //Toast.makeText(getApplicationContext(), "Cancel Click", Toast.LENGTH_SHORT).show();
            }
        });

        builder.setPositiveButton("등록", new DialogInterface.OnClickListener(){ //확인버튼
            @Override
            public void onClick(DialogInterface dialog, int id) {
                boolean check = true; //리스트 중복 제거

                for (int i = 0; i < midList.size(); i++) {
                    if(midList.get(i).equals(macAddress)) {
                        //check = false;
                        break;
                    }
                }

                if (check) {
                    sendData(macAddress);
                    mod = 2;
                    Toast.makeText(getApplicationContext(), "등록 되었습니다", Toast.LENGTH_SHORT).show();
                }
                else {
                    Toast.makeText(getApplicationContext(), "이미 등록된 MAC 주소 입니다", Toast.LENGTH_SHORT).show();
                }
            }
        });

        AlertDialog alertDialog = builder.create();
        alertDialog.show();
    }
}
