package com.mycompany.myapp;

import android.app.*;
import android.bluetooth.*;
import android.content.*;
import android.os.*;
import android.widget.*;
import java.util.*;
import java.io.*;

public class MainActivity extends Activity 
{

	private static final int REQUEST_ENABLE_BT = 1;
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
		TextView textview = (TextView)findViewById(R.id.myTextView);
	
		try
		{
		    // Get the bluetooth adapter.
			BluetoothAdapter bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
			if (bluetoothAdapter == null)
                throw new Exception("No bluetooth adapter");

			// Check the adapter is enabled.
			if (!bluetoothAdapter.isEnabled())
			{
				Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
				startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
			}
			if (!bluetoothAdapter.isEnabled())
				throw new Exception("Bluetooth adapter not enabled");

			textview.append("Bluetooth enabled\n");
		
			Set<BluetoothDevice> pairedDevices = bluetoothAdapter.getBondedDevices();
			BluetoothDevice arduino = null;
			List<String> devices = new ArrayList<>();
			if (pairedDevices.size() > 0)
			{
				// There are paired devices. Get the name and address of each paired device.
				for (BluetoothDevice device : pairedDevices)
				{
					String deviceName = device.getName();
					if (deviceName.contentEquals("LinkSprite"))
                    {
                        textview.append("Found: " + deviceName + " {" + device.getAddress() + "}\n");
						arduino = device;
                    }
				}
			}
			if (arduino == null)
                throw new Exception("Couldn't find LinkSprite");

			UUID SPP_UUID = java.util.UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");
			BluetoothSocket socket = arduino.createRfcommSocketToServiceRecord(SPP_UUID);
			try
			{
				socket.connect();
			}
			catch (IOException e)
			{
				throw new Exception(e.getMessage());
			}
			textview.append("Bluetooth socket connected\n");

			String outString = "SOS\0";
			byte[] output = {(byte)0,(byte)1,(byte)0};
            socket.getOutputStream().write(outString.getBytes());

            Thread.sleep(1000);

			InputStream inStream = socket.getInputStream();
			int nAvailableBytes = inStream.available();
            byte[] buffer = new byte[255];
            inStream.read(buffer);
            String returnedString = "";
            for (int i = 0 ; i < nAvailableBytes ; i++)
                returnedString += String.format("%c",buffer[i]);
			textview.append(returnedString);

			socket.close();
			textview.append("Bluetooth socket closed");
		}
		catch (Exception e)
		{
			textview.append(e.getMessage());
		}
    }
}

