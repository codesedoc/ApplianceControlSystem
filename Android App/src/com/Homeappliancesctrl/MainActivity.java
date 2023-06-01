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
	//界面控件对应的成员
	private Button connectBtn;//连接断开按钮
	private TextView titleTextView;//头部的提示TextView
	private LinearLayout otherLL;//中间用于动态布局的LinearLayout
	
	//用于蓝牙建立连接和通信的成员
	protected static final int REQUEST_CONNECTdevice = 1;//搜索设备界面的跳转标记值
	protected static final int REQUEST_CTRLdevice = 2;//搜索设备界面的跳转标记值
	
	private BluetoothAdapter bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();//获取本地蓝牙适配器，即蓝牙设备
	BluetoothSocket bluetoothSocket = null;//蓝牙通信socket
    boolean bingRecThread = false;//标记是否已经启动接受线程
    private final static String MY_UUID = "00001101-0000-1000-8000-00805F9B34FB";   //SPP服务UUID号
	private InputStream btSocketIS;//socket对应输入流
	private OutputStream btSocketOS;//socket对应输出流
	private String fmsg="";
	protected String smsg="";
	
	//处理消息队列handler
	private Handler handler= new Handler(){
    	public void handleMessage(Message msg){
    		super.handleMessage(msg);
    		Toast.makeText(MainActivity.this, fmsg, Toast.LENGTH_SHORT).show();//显示数据 
    	}
    };
	//接收数据线程 
	Thread btReceiveThread=new Thread(){
		public void run(){
    		int num = 0;
    		byte[] buffer = new byte[1024];
    		byte[] buffer_new = new byte[1024];
    		int i = 0;
    		int n = 0;
    		//接收线程
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
    					num = btSocketIS.read(buffer);         //读入数据
    					n=0;
    					
    					String s0 = new String(buffer,0,num);
    					fmsg+=s0;    //保存收到数据
    					//Toast.makeText(MainActivity.this, s0, Toast.LENGTH_SHORT).show();//显示数据 
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
//    					smsg+=s;   //写入接收缓存
    					if(btSocketIS.available()==0)break;  //短时间没有数据才跳出进行显示
    				}
    				//发送显示消息，进行显示刷新
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
      //如果打开本地蓝牙设备不成功，提示信息，结束程序
        if (bluetoothAdapter == null){
        	Toast.makeText(this, "无法打开手机蓝牙，请确认手机是否有蓝牙功能！", Toast.LENGTH_LONG).show();
            finish();
            return;
        }
        
        // 设置手机蓝牙使能
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
				if(bluetoothAdapter.isEnabled()==false){  //如果蓝牙服务不可用则提示
		    		Toast.makeText(MainActivity.this, "打开蓝牙中...", Toast.LENGTH_LONG).show(); 
		    		return;
		    	}
				//没有连接执行连接相关操作
				if (bluetoothSocket==null) {
					Intent intent = new Intent(MainActivity.this, GetBTDevice.class); //跳转程序设置
			    	startActivityForResult(intent, REQUEST_CONNECTdevice);  //设置返回宏定义
				}
				//已经连接则断开
				else{
		    		 //关闭连接socket
		    	    try{
		    	    	
		    	    	btSocketIS.close();
		    	    	bluetoothSocket.close();
		    	    	bluetoothSocket = null;
		    	    	connectBtn.setText("连接");
		    	    	titleTextView.setText("未连接红外控制器！");
		    	    	titleTextView.setTextColor(Color.rgb(0, 0, 255));
		    	    	otherLL.removeAllViews();
		    	    }catch(IOException e){}   
		    	}
		    	return;
		    	
				
			}
		});
        
        
        //注册蓝牙连接状态广播监听
        registerReceiver(btDeviceCSReceiver, new IntentFilter(BluetoothDevice.ACTION_ACL_DISCONNECTED));
    }
    //从搜索界面返回时执行onActivityResult方法
    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
    	super.onActivityResult(requestCode, resultCode, data);
    	//过滤从其他界面返回的情况
    	if(requestCode==REQUEST_CONNECTdevice && resultCode==Activity.RESULT_OK){
    		sokectConnect(data);
    	}
    	
    	else if (requestCode==REQUEST_CTRLdevice && resultCode==Activity.RESULT_OK) {
			
		}
    	
    }
  //定义电视机/机顶盒遥控所有按键对应的控件变量，并初始化
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
		View heCtrlBtnsItem;//自定义的一个通用View，存放动态或得一个遥控器按钮布局，被用于添加到LinearLayout中
		heCtrlBtnsItem = LayoutInflater.from(MainActivity.this).inflate(R.layout.he_tv_contrlbtns, null);
		otherLL.addView(heCtrlBtnsItem);
		
    	for(int i=0;i<27;i++){
    		heTVBtnsOnclick(heCtrlBtnsItem,i);
    	}
    	
		titleTextView.setText("电视/机顶盒遥控");
	}
    private void heTVBtnsOnclick(View heCtrlBtnsItem,final int residIndex){	
    	ImageButton btns=(ImageButton) heCtrlBtnsItem.findViewById(TVbtnsResId[residIndex]);//获取遥控按钮
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
  		View heCtrlBtnsItem;//自定义的一个通用View，存放动态或得一个遥控器按钮布局，被用于添加到LinearLayout中
  		heCtrlBtnsItem = LayoutInflater.from(MainActivity.this).inflate(R.layout.he_air_contrlbtns, null);
  		otherLL.addView(heCtrlBtnsItem);
  		titleTextView.setText("空调遥控");
  	}
    
    private void sokectConnect(Intent data){
    	BluetoothDevice device=null;//定义临时存放远程蓝牙设备的成员
    	//如果设备名称不对则提示不建立连接，并作出对应提示
    	String name=data.getExtras().getString(GetBTDevice.EXTRA_DEVICE_NAME);
    	if(!name.equals("FraredCtrl-XS1")){
    		Toast.makeText(this, name+"不正确!", Toast.LENGTH_SHORT).show();
    		titleTextView.setText("所连设备不正确！");//更改头部提示信息和颜色
    		titleTextView.setTextColor(Color.rgb(255, 0, 0));
    		connectBtn.setText("重新连接");//更改按钮的文字提示
    		otherLL.removeAllViews();//清除中间动态布局中的控件
    		return;
    	}
    	//通过蓝牙设配器和搜索设备返回的设备mac地址获取蓝牙设备
    	device=bluetoothAdapter.getRemoteDevice(data.getExtras().getString(GetBTDevice.EXTRA_DEVICE_ADDRESS));
    	//用指定的服务号将所选蓝牙设备绑定socket
        try{
        	bluetoothSocket = device.createRfcommSocketToServiceRecord(UUID.fromString(MY_UUID));
        }catch(IOException e){
        	//如果出现异常则还原界面初始状态
        	Toast.makeText(this, "连接失败！", Toast.LENGTH_SHORT).show();
        	titleTextView.setText("未连接红外控制器！");
        	titleTextView.setTextColor(Color.rgb(0, 0, 255));
        	otherLL.removeAllViews();
        }
        //建立socket（所选蓝牙设备）连接
        try{
        	bluetoothSocket.connect();
        	//连接成功后更新界面相应的提示
        	Toast.makeText(this, "连接"+device.getName()+"成功！", Toast.LENGTH_SHORT).show();
        	titleTextView.setText("已连接红外控制器！");
        	titleTextView.setTextColor(Color.rgb(0, 255, 0));
        	connectBtn.setText("断开");
        	otherLL.removeAllViews();//清除之间的动态控件
        	//该界面控制按钮用于点击后将动态界面变为电视或机顶盒遥控层
        	ctrlBtnItemTV = LayoutInflater.from(MainActivity.this).inflate(R.layout.contrlbtn_item, null);
        	TextView TVCtrlBtn=(TextView) ctrlBtnItemTV.findViewById(R.id.ctrlBtn);//获取动态按钮
			TVCtrlBtn.setText("电视机/机顶盒控制");//更改按钮提示文字
			//该界面控制按钮用于点击后将动态界面变为空调遥控层
        	ctrlBtnItemAir = LayoutInflater.from(MainActivity.this).inflate(R.layout.contrlbtn_item, null);
			TextView airCtrlBtn=(TextView) ctrlBtnItemAir.findViewById(R.id.ctrlBtn);
			airCtrlBtn.setText("空调控制");
			//该界面控制按钮用于点击后将动态界面从各遥控层返回
			ctrlBtnItemBack = LayoutInflater.from(MainActivity.this).inflate(R.layout.contrlbtn_item, null);
			TextView backCtrlBtn=(TextView) ctrlBtnItemBack.findViewById(R.id.ctrlBtn);//获取动态按钮
			backCtrlBtn.setText("返回");//更改按钮提示文字
     
			otherLL.addView(ctrlBtnItemTV);//加入屏幕中间的布局中
			otherLL.addView(ctrlBtnItemAir);
		
			//为各个动态界面控制按钮添加点击事件响应
			//为电视机按钮添加点击事件响应
			TVCtrlBtn.setOnClickListener(new OnClickListener() {
				@Override
				public void onClick(View v) {
					heTVCtrl();//进入电视机/机顶盒遥控处理函数
				}
			});
			//为空调按钮添加点击事件响应
//			airCtrlBtn.setOnClickListener(new OnClickListener() {
//				@Override
//				public void onClick(View v) {
////					try {
////						btSocketOS.write("456".getBytes());
////					} catch (IOException e) {
////						e.printStackTrace();
////					}
//					heAirCtrl();//进入空调遥控处理函数
//				}	
//			});
			
			backCtrlBtn.setOnClickListener(new OnClickListener() {
				
				@Override
				public void onClick(View v) {
					//按下返回键时还原动态界面初始状态
					otherLL.removeAllViews();
					otherLL.addView(ctrlBtnItemTV);
					otherLL.addView(ctrlBtnItemAir);
					
					titleTextView.setText("设备工作正常");
				}
			});
			
        }catch(IOException e){
        	//连接失败回复初状态
        	try{
        		Toast.makeText(this, "连接失败！", Toast.LENGTH_SHORT).show();
        		titleTextView.setText("未连接红外控制器！");
        		titleTextView.setTextColor(Color.rgb(0, 0, 255));
        		otherLL.removeAllViews();
        		bluetoothSocket.close();
        		bluetoothSocket = null;
        	}catch(IOException ee){
        		Toast.makeText(this, "连接失败！", Toast.LENGTH_SHORT).show();
        		titleTextView.setText("未连接红外控制器！");
        		titleTextView.setTextColor(Color.rgb(0, 0, 255));
        		otherLL.removeAllViews();
        	}
        	
        	return;
        }
        //连接成功后建立输入输出流
        try {
			btSocketOS= bluetoothSocket.getOutputStream();//建立对应socket的输出流
		} catch (IOException e1) {
			Toast.makeText(this, "不能发送数据！", Toast.LENGTH_SHORT).show();
		}
        try{
        	btSocketIS = bluetoothSocket.getInputStream();//建立对应socket的输入流
    		}catch(IOException e){
    			Toast.makeText(this, "不能接收数据！", Toast.LENGTH_SHORT).show();
    			return;
    	}
       
//        //打开接收线程
//        if(bingRecThread==false){
//    		btReceiveThread.start();//没有启动接收线程则启动
//    		bingRecThread=true;//启动否置标记值防止下次重复启动
//    	}			
    }
    //关闭程序掉用处理部分
    public void onDestroy(){
    	super.onDestroy();
    	if(bluetoothSocket!=null)  
    	//关闭连接socket
    	try{
    		bluetoothSocket.close();
    	}catch(IOException e){}
    	//关闭连接输入输出流
    	try {
    		btSocketIS.close();
			btSocketOS.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
    	
    }
    
    
  

    //定义蓝牙设备连接状态广播监听器及其实现的抽象方法
    private BroadcastReceiver btDeviceCSReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
        	//如果是主动断开的连接则bluetoothSocket为空null
        	//只有异常的断开连接才提示
        	if (bluetoothSocket!=null) {
        		titleTextView.setText("连接被断开！");//更改头部提示信息和颜色
        		titleTextView.setTextColor(Color.rgb(255, 0, 0));
        		connectBtn.setText("重新连接");//更改按钮的文字提示
        		otherLL.removeAllViews();//清除中间动态布局中的控件
            	//Toast.makeText(MainActivity.this, "STATE_OFF", Toast.LENGTH_SHORT).show();
        	}
        }
    };
}
