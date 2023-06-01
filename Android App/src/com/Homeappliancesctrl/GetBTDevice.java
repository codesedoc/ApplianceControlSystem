package com.Homeappliancesctrl;

import android.R.integer;
import android.app.Activity;
import android.app.ListActivity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.Window;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.AdapterView.OnItemClickListener;

public class GetBTDevice extends Activity{

	// ����ʱ���ݱ�ǩ
    public static String EXTRA_DEVICE_ADDRESS = "�豸��ַ";
    public static String EXTRA_DEVICE_NAME = "�豸����";
    
    // ��Ա��
    private BluetoothAdapter mBtAdapter;
    private ArrayAdapter<String> mPairedDevicesArrayAdapter;
    private ArrayAdapter<String> mNewDevicesArrayAdapter;
    
    
    //ѡ���豸��Ӧ����������ʵ�ֵĳ��󷽷�
    private OnItemClickListener mDeviceClickListener = new OnItemClickListener() {
        public void onItemClick(AdapterView<?> av, View v, int arg2, long arg3) {
            // ׼�������豸���ر��豸ɨ��
            mBtAdapter.cancelDiscovery();

            // �õ�mac��ַ
            String info = ((TextView) v).getText().toString();
            String address = info.substring(info.length() - 17);
            
            // �õ��豸����
            String name = info.substring(0,info.length() - 17).trim();
            
            // ���÷�������
            Intent intent = new Intent();
            intent.putExtra(EXTRA_DEVICE_ADDRESS, address);
            intent.putExtra(EXTRA_DEVICE_NAME, name);
            
            // ���÷���ֵ����������
            setResult(Activity.RESULT_OK, intent);
            finish();
        }
    };
 // ���ҵ��豸���������action����������ʵ�ֵĳ��󷽷�
    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            // ���ҵ��豸action
            if (BluetoothDevice.ACTION_FOUND.equals(action)) {
                // �õ������豸
                BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                // ���������Ե����Թ����ѵõ���ʾ�����������ӵ��б��н�����ʾ
                if (device.getBondState() != BluetoothDevice.BOND_BONDED) {
                    mNewDevicesArrayAdapter.add(device.getName() + "\n" + device.getAddress());
                }else{  //��ӵ�������豸�б�
                	mPairedDevicesArrayAdapter.add(device.getName() + "\n" + device.getAddress());
                }
            // �������action
            } else if (BluetoothAdapter.ACTION_DISCOVERY_FINISHED.equals(action)) {
                setProgressBarIndeterminateVisibility(false);
                setTitle("ѡ��Ҫ���ӵ��豸");
                if (mNewDevicesArrayAdapter.getCount() == 0) {
                    String noDevices = "û���ҵ����豸";
                    mNewDevicesArrayAdapter.add(noDevices);
                }
            }
        }
    };
    
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		 // ��������ʾ����(���ֲ�ȷ���Ľ���)
        requestWindowFeature(Window.FEATURE_INDETERMINATE_PROGRESS);  //���ô�����ʾģʽΪ���ڷ�ʽ
		setContentView(R.layout.device_list);
		 // �趨Ĭ�Ϸ���ֵΪȡ��
        setResult(Activity.RESULT_CANCELED);
        
     // �趨ɨ�谴����Ӧ
        Button scanButton = (Button) findViewById(R.id.button_scan);
        scanButton.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                doDiscovery();
                v.setVisibility(v.GONE);
            }
        });
        
        // �õ������������
        mBtAdapter = BluetoothAdapter.getDefaultAdapter();
        // ��ʹ���豸�洢����
        mPairedDevicesArrayAdapter = new ArrayAdapter<String>(this, R.layout.device_name);
        mNewDevicesArrayAdapter = new ArrayAdapter<String>(this, R.layout.device_name);
        
        // ����������豸�б�
        ListView pairedListView = (ListView) findViewById(R.id.paired_devices);
        pairedListView.setAdapter(mPairedDevicesArrayAdapter);
        pairedListView.setOnItemClickListener(mDeviceClickListener);

        // �����²����豸�б�
        ListView newDevicesListView = (ListView) findViewById(R.id.new_devices);
        newDevicesListView.setAdapter(mNewDevicesArrayAdapter);
        newDevicesListView.setOnItemClickListener(mDeviceClickListener);

        // ע����ղ��ҵ��豸action������
        //IntentFilter filter = new IntentFilter(BluetoothDevice.ACTION_FOUND);
        this.registerReceiver(mReceiver, new IntentFilter(BluetoothDevice.ACTION_FOUND));
        // ע����ҽ���action������
        this.registerReceiver(mReceiver, new IntentFilter(BluetoothAdapter.ACTION_DISCOVERY_FINISHED));
	}
	/*��ʼɨ����Χ�����豸*/
    private void doDiscovery() {
        // �ڴ�����ʾ��������Ϣ
        setProgressBarIndeterminateVisibility(true);
        setTitle("�����豸��...");

        // ��ʾ�����豸��δ����豸���б�
        findViewById(R.id.title_new_devices).setVisibility(View.VISIBLE);

        // �ر��ڽ��еķ������
        if (mBtAdapter.isDiscovering()) {
            mBtAdapter.cancelDiscovery();
        }
        //�����¿�ʼ
        mBtAdapter.startDiscovery();
    }
    
    
}
