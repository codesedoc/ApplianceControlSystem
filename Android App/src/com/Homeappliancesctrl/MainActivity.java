package com.Homeappliancesctrl;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.UUID;


import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.Color;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;

public class MainActivity extends Activity {
	//����ؼ���Ӧ�ĳ�Ա
	private Button connectBtn;//���ӶϿ���ť
	private TextView titleTextView;//ͷ������ʾTextView
	private LinearLayout otherLL;//�м����ڶ�̬���ֵ�LinearLayout
	
	//���������������Ӻ�ͨ�ŵĳ�Ա
	protected static final int REQUEST_CONNECTdevice = 1;//�����豸�������ת���ֵ
	protected static final int REQUEST_CTRLdevice = 2;//�����豸�������ת���ֵ
	
	private BluetoothAdapter bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();//��ȡ�����������������������豸
	BluetoothSocket bluetoothSocket = null;//����ͨ��socket
    boolean bingRecThread = false;//����Ƿ��Ѿ����������߳�
    private final static String MY_UUID = "00001101-0000-1000-8000-00805F9B34FB";   //SPP����UUID��
	private InputStream btSocketIS;//socket��Ӧ������
	private OutputStream btSocketOS;//socket��Ӧ�����
	private String fmsg="";
	protected String smsg="";
	
	//������Ϣ����handler
	private Handler handler= new Handler(){
    	public void handleMessage(Message msg){
    		super.handleMessage(msg);
    		Toast.makeText(MainActivity.this, fmsg, Toast.LENGTH_SHORT).show();//��ʾ���� 
    	}
    };
	//���������߳� 
	Thread btReceiveThread=new Thread(){
		public void run(){
    		int num = 0;
    		byte[] buffer = new byte[1024];
    		byte[] buffer_new = new byte[1024];
    		int i = 0;
    		int n = 0;
    		//�����߳�
    		while(true){
    			int a=0;
    			a++;
    			if (btSocketIS==null) {
    				return;
				}
    			try{
    				while(btSocketIS.available()==0){
    		
    				}
    				while(true){
    					num = btSocketIS.read(buffer);         //��������
    					n=0;
    					
    					String s0 = new String(buffer,0,num);
    					fmsg+=s0;    //�����յ�����
    					//Toast.makeText(MainActivity.this, s0, Toast.LENGTH_SHORT).show();//��ʾ���� 
//    					for(i=0;i<num;i++){
//    						if((buffer[i] == 0x0d)&&(buffer[i+1]==0x0a)){
//    							buffer_new[n] = 0x0a;
//    							i++;
//    						}else{
//    							buffer_new[n] = buffer[i];
//    						}
//    						n++;
//    					}
//    					String s = new String(buffer_new,0,n);
//    					smsg+=s;   //д����ջ���
    					if(btSocketIS.available()==0)break;  //��ʱ��û�����ݲ�����������ʾ
    				}
    				//������ʾ��Ϣ��������ʾˢ��
    					handler.sendMessage(handler.obtainMessage());       	    		
    	    		}catch(IOException e){
    	    	}
    		}
    	}
    };
	private View ctrlBtnItemTV;
	private View ctrlBtnItemAir;
	private View ctrlBtnItemBack;
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        connectBtn=(Button) findViewById(R.id.connectBtn);
        titleTextView=(TextView) findViewById(R.id.mainTv);
        otherLL=(LinearLayout) findViewById(R.id.other_ll);
      //����򿪱��������豸���ɹ�����ʾ��Ϣ����������
        if (bluetoothAdapter == null){
        	Toast.makeText(this, "�޷����ֻ���������ȷ���ֻ��Ƿ����������ܣ�", Toast.LENGTH_LONG).show();
            finish();
            return;
        }
        
        // �����ֻ�����ʹ��
       new Thread(){
    	   public void run(){
    		   if(bluetoothAdapter.isEnabled()==false){
        		bluetoothAdapter.enable();
    		   }
    	   }   	   
       }.start();      
        
        connectBtn.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View v) {
				if(bluetoothAdapter.isEnabled()==false){  //����������񲻿�������ʾ
		    		Toast.makeText(MainActivity.this, "��������...", Toast.LENGTH_LONG).show(); 
		    		return;
		    	}
				//û������ִ��������ز���
				if (bluetoothSocket==null) {
					Intent intent = new Intent(MainActivity.this, GetBTDevice.class); //��ת��������
			    	startActivityForResult(intent, REQUEST_CONNECTdevice);  //���÷��غ궨��
				}
				//�Ѿ�������Ͽ�
				else{
		    		 //�ر�����socket
		    	    try{
		    	    	
		    	    	btSocketIS.close();
		    	    	bluetoothSocket.close();
		    	    	bluetoothSocket = null;
		    	    	connectBtn.setText("����");
		    	    	titleTextView.setText("δ���Ӻ����������");
		    	    	titleTextView.setTextColor(Color.rgb(0, 0, 255));
		    	    	otherLL.removeAllViews();
		    	    }catch(IOException e){}   
		    	}
		    	return;
		    	
				
			}
		});
        
        
        //ע����������״̬�㲥����
        registerReceiver(btDeviceCSReceiver, new IntentFilter(BluetoothDevice.ACTION_ACL_DISCONNECTED));
    }
    //���������淵��ʱִ��onActivityResult����
    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
    	super.onActivityResult(requestCode, resultCode, data);
    	//���˴��������淵�ص����
    	if(requestCode==REQUEST_CONNECTdevice && resultCode==Activity.RESULT_OK){
    		sokectConnect(data);
    	}
    	
    	else if (requestCode==REQUEST_CTRLdevice && resultCode==Activity.RESULT_OK) {
			
		}
    	
    }
  //������ӻ�/������ң�����а�����Ӧ�Ŀؼ�����������ʼ��
	int TVbtnsResId[]=new int[30];
	
    private void heTVCtrl() {
    	
    	TVbtnsResId[0]=R.id.TVpowerBtn; 
    	TVbtnsResId[1]=R.id.TVsilenceBtn; 
    	TVbtnsResId[2]=R.id.TVnum0Btn;
    	TVbtnsResId[3]=R.id.TVnum1Btn; 
    	TVbtnsResId[4]=R.id.TVnum2Btn; 
    	TVbtnsResId[5]=R.id.TVnum3Btn; 
    	TVbtnsResId[6]=R.id.TVnum4Btn;
    	TVbtnsResId[7]=R.id.TVnum5Btn;
    	TVbtnsResId[8]=R.id.TVnum6Btn;
    	TVbtnsResId[9]=R.id.TVnum7Btn;
    	TVbtnsResId[10]=R.id.TVnum8Btn; 
    	TVbtnsResId[11]=R.id.TVnum9Btn;
    	TVbtnsResId[12]=R.id.TVtvBtn; 
    	TVbtnsResId[13]=R.id.TVmanyBtn;
    	TVbtnsResId[14]=R.id.TVupBtn; 
    	TVbtnsResId[15]=R.id.TVdownBtn;
    	TVbtnsResId[16]=R.id.TVokBtn; 
    	TVbtnsResId[17]=R.id.TVleftBtn; 
    	TVbtnsResId[18]=R.id.TVrightBtn; 
    	TVbtnsResId[19]=R.id.TVhomeBtn; 
    	TVbtnsResId[20]=R.id.TVmenuBtn;
    	TVbtnsResId[21]=R.id.TVsetBtn; 
    	TVbtnsResId[22]=R.id.TVbackBtn;
    	TVbtnsResId[23]=R.id.TVchannelAddBtn; 
    	TVbtnsResId[24]=R.id.TVchannelSubBtn;
    	TVbtnsResId[25]=R.id.TVvoiceAddBtn; 
    	TVbtnsResId[26]=R.id.TVvoiceSubBtn;
    	
    	otherLL.removeAllViews();
		otherLL.addView(ctrlBtnItemBack);
		View heCtrlBtnsItem;//�Զ����һ��ͨ��View����Ŷ�̬���һ��ң������ť���֣���������ӵ�LinearLayout��
		heCtrlBtnsItem = LayoutInflater.from(MainActivity.this).inflate(R.layout.he_tv_contrlbtns, null);
		otherLL.addView(heCtrlBtnsItem);
		
    	for(int i=0;i<27;i++){
    		heTVBtnsOnclick(heCtrlBtnsItem,i);
    	}
    	
		titleTextView.setText("����/������ң��");
	}
    private void heTVBtnsOnclick(View heCtrlBtnsItem,final int residIndex){	
    	ImageButton btns=(ImageButton) heCtrlBtnsItem.findViewById(TVbtnsResId[residIndex]);//��ȡң�ذ�ť
		btns.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				// TODO Auto-generated method stub
				try {
					byte temp[]={1,0};
					temp[1]=(byte)(residIndex+1);
					btSocketOS.write(temp);
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
		});
    }
    private void heAirCtrl() {
    	otherLL.removeAllViews();
		otherLL.addView(ctrlBtnItemBack);
  		View heCtrlBtnsItem;//�Զ����һ��ͨ��View����Ŷ�̬���һ��ң������ť���֣���������ӵ�LinearLayout��
  		heCtrlBtnsItem = LayoutInflater.from(MainActivity.this).inflate(R.layout.he_air_contrlbtns, null);
  		otherLL.addView(heCtrlBtnsItem);
  		titleTextView.setText("�յ�ң��");
  	}
    
    private void sokectConnect(Intent data){
    	BluetoothDevice device=null;//������ʱ���Զ�������豸�ĳ�Ա
    	//����豸���Ʋ�������ʾ���������ӣ���������Ӧ��ʾ
    	String name=data.getExtras().getString(GetBTDevice.EXTRA_DEVICE_NAME);
    	if(!name.equals("FraredCtrl-XS1")){
    		Toast.makeText(this, name+"����ȷ!", Toast.LENGTH_SHORT).show();
    		titleTextView.setText("�����豸����ȷ��");//����ͷ����ʾ��Ϣ����ɫ
    		titleTextView.setTextColor(Color.rgb(255, 0, 0));
    		connectBtn.setText("��������");//���İ�ť��������ʾ
    		otherLL.removeAllViews();//����м䶯̬�����еĿؼ�
    		return;
    	}
    	//ͨ�������������������豸���ص��豸mac��ַ��ȡ�����豸
    	device=bluetoothAdapter.getRemoteDevice(data.getExtras().getString(GetBTDevice.EXTRA_DEVICE_ADDRESS));
    	//��ָ���ķ���Ž���ѡ�����豸��socket
        try{
        	bluetoothSocket = device.createRfcommSocketToServiceRecord(UUID.fromString(MY_UUID));
        }catch(IOException e){
        	//��������쳣��ԭ�����ʼ״̬
        	Toast.makeText(this, "����ʧ�ܣ�", Toast.LENGTH_SHORT).show();
        	titleTextView.setText("δ���Ӻ����������");
        	titleTextView.setTextColor(Color.rgb(0, 0, 255));
        	otherLL.removeAllViews();
        }
        //����socket����ѡ�����豸������
        try{
        	bluetoothSocket.connect();
        	//���ӳɹ�����½�����Ӧ����ʾ
        	Toast.makeText(this, "����"+device.getName()+"�ɹ���", Toast.LENGTH_SHORT).show();
        	titleTextView.setText("�����Ӻ����������");
        	titleTextView.setTextColor(Color.rgb(0, 255, 0));
        	connectBtn.setText("�Ͽ�");
        	otherLL.removeAllViews();//���֮��Ķ�̬�ؼ�
        	//�ý�����ư�ť���ڵ���󽫶�̬�����Ϊ���ӻ������ң�ز�
        	ctrlBtnItemTV = LayoutInflater.from(MainActivity.this).inflate(R.layout.contrlbtn_item, null);
        	TextView TVCtrlBtn=(TextView) ctrlBtnItemTV.findViewById(R.id.ctrlBtn);//��ȡ��̬��ť
			TVCtrlBtn.setText("���ӻ�/�����п���");//���İ�ť��ʾ����
			//�ý�����ư�ť���ڵ���󽫶�̬�����Ϊ�յ�ң�ز�
        	ctrlBtnItemAir = LayoutInflater.from(MainActivity.this).inflate(R.layout.contrlbtn_item, null);
			TextView airCtrlBtn=(TextView) ctrlBtnItemAir.findViewById(R.id.ctrlBtn);
			airCtrlBtn.setText("�յ�����");
			//�ý�����ư�ť���ڵ���󽫶�̬����Ӹ�ң�ز㷵��
			ctrlBtnItemBack = LayoutInflater.from(MainActivity.this).inflate(R.layout.contrlbtn_item, null);
			TextView backCtrlBtn=(TextView) ctrlBtnItemBack.findViewById(R.id.ctrlBtn);//��ȡ��̬��ť
			backCtrlBtn.setText("����");//���İ�ť��ʾ����
     
			otherLL.addView(ctrlBtnItemTV);//������Ļ�м�Ĳ�����
			otherLL.addView(ctrlBtnItemAir);
		
			//Ϊ������̬������ư�ť��ӵ���¼���Ӧ
			//Ϊ���ӻ���ť��ӵ���¼���Ӧ
			TVCtrlBtn.setOnClickListener(new OnClickListener() {
				@Override
				public void onClick(View v) {
					heTVCtrl();//������ӻ�/������ң�ش�����
				}
			});
			//Ϊ�յ���ť��ӵ���¼���Ӧ
//			airCtrlBtn.setOnClickListener(new OnClickListener() {
//				@Override
//				public void onClick(View v) {
////					try {
////						btSocketOS.write("456".getBytes());
////					} catch (IOException e) {
////						e.printStackTrace();
////					}
//					heAirCtrl();//����յ�ң�ش�����
//				}	
//			});
			
			backCtrlBtn.setOnClickListener(new OnClickListener() {
				
				@Override
				public void onClick(View v) {
					//���·��ؼ�ʱ��ԭ��̬�����ʼ״̬
					otherLL.removeAllViews();
					otherLL.addView(ctrlBtnItemTV);
					otherLL.addView(ctrlBtnItemAir);
					
					titleTextView.setText("�豸��������");
				}
			});
			
        }catch(IOException e){
        	//����ʧ�ܻظ���״̬
        	try{
        		Toast.makeText(this, "����ʧ�ܣ�", Toast.LENGTH_SHORT).show();
        		titleTextView.setText("δ���Ӻ����������");
        		titleTextView.setTextColor(Color.rgb(0, 0, 255));
        		otherLL.removeAllViews();
        		bluetoothSocket.close();
        		bluetoothSocket = null;
        	}catch(IOException ee){
        		Toast.makeText(this, "����ʧ�ܣ�", Toast.LENGTH_SHORT).show();
        		titleTextView.setText("δ���Ӻ����������");
        		titleTextView.setTextColor(Color.rgb(0, 0, 255));
        		otherLL.removeAllViews();
        	}
        	
        	return;
        }
        //���ӳɹ��������������
        try {
			btSocketOS= bluetoothSocket.getOutputStream();//������Ӧsocket�������
		} catch (IOException e1) {
			Toast.makeText(this, "���ܷ������ݣ�", Toast.LENGTH_SHORT).show();
		}
        try{
        	btSocketIS = bluetoothSocket.getInputStream();//������Ӧsocket��������
    		}catch(IOException e){
    			Toast.makeText(this, "���ܽ������ݣ�", Toast.LENGTH_SHORT).show();
    			return;
    	}
       
//        //�򿪽����߳�
//        if(bingRecThread==false){
//    		btReceiveThread.start();//û�����������߳�������
//    		bingRecThread=true;//�������ñ��ֵ��ֹ�´��ظ�����
//    	}			
    }
    //�رճ�����ô�����
    public void onDestroy(){
    	super.onDestroy();
    	if(bluetoothSocket!=null)  
    	//�ر�����socket
    	try{
    		bluetoothSocket.close();
    	}catch(IOException e){}
    	//�ر��������������
    	try {
    		btSocketIS.close();
			btSocketOS.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
    	
    }
    
    
  

    //���������豸����״̬�㲥����������ʵ�ֵĳ��󷽷�
    private BroadcastReceiver btDeviceCSReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
        	//����������Ͽ���������bluetoothSocketΪ��null
        	//ֻ���쳣�ĶϿ����Ӳ���ʾ
        	if (bluetoothSocket!=null) {
        		titleTextView.setText("���ӱ��Ͽ���");//����ͷ����ʾ��Ϣ����ɫ
        		titleTextView.setTextColor(Color.rgb(255, 0, 0));
        		connectBtn.setText("��������");//���İ�ť��������ʾ
        		otherLL.removeAllViews();//����м䶯̬�����еĿؼ�
            	//Toast.makeText(MainActivity.this, "STATE_OFF", Toast.LENGTH_SHORT).show();
        	}
        }
    };
}
