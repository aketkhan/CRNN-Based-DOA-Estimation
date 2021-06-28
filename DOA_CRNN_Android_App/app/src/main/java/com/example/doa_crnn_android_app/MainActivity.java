package com.example.doa_crnn_android_app;


import android.Manifest;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.pm.PackageManager;
import android.graphics.Color;
import android.media.MediaPlayer;
import android.os.Bundle;
import android.os.CountDownTimer;
import android.os.Environment;

import android.text.InputType;
import android.util.Log;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.TextView;
import android.content.Context;
import android.widget.Toast;

import java.io.File;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.text.DecimalFormat;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.atomic.AtomicBoolean;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import static android.content.ContentValues.TAG;





public class MainActivity extends AppCompatActivity
        implements ActivityCompat.OnRequestPermissionsResultCallback {
    private int mode;
    private String dialogFile;
    private AtomicBoolean processingFlag;
    private RadioGroup myradiogroup1;
    private RadioGroup myradiogroup2;
    private RadioButton rbEnhanced;
    private RadioButton rbOriginal;

    private RadioButton rbBabble;
    private RadioButton rbDrivingcar;
    private RadioButton rbMachinery;
    private EditText etFrameTime;
    public EditText etAngle;
    public EditText etSFxavg;
    public EditText etThreadDuration;
    private WaveRecorder waveRecorder;

    // private AudioReader audioReader;
    private BlockingQueue<AudioFrame> inputFrames;
    // private BlockingQueue<AudioFrame> inputFrames2;
    private BlockingQueue<AudioFrame> processedFrames;
    private int noisetype;
    private int playback;
    private static Context mContext;
    public Processing ake_processing;
    private boolean isResultSaver=false;
    public TextView ake_orientation;
    private SignalProcessing signalProcessing;
    AlertDialog.Builder alert;
    EditText input;
    String file_name;
    Context main_context;
    File gpxfile ;



    /*, R.raw.degree_40,
            R.raw.degree_60, R.raw.degree_80, R.raw.degree_100, R.raw.degree_120,
            R.raw.degree_140 , R.raw.degree_160, R.raw.degree_180 */


    private static final int AUDIO_ECHO_REQUEST = 0;

    public MainActivity() {
    }


    //private static MainActivity ins;


    public static Context getContext() {
        Log.i(TAG, "getContext: " + mContext);
        return mContext;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        // getSupportActionBar().hide(); // for hiding title

        setContentView(R.layout.activity_main);


        Settings.setCallbackInterface(this);

        //Utilities.prepareDirectory(getString(R.string.appDirectory));

        inputFrames = new ArrayBlockingQueue<AudioFrame>(Settings.queueSize);
        // inputFrames2 = new ArrayBlockingQueue<AudioFrame>(Settings.queueSize);
        processedFrames = new ArrayBlockingQueue<AudioFrame>(Settings.queueSize);
        processingFlag = new AtomicBoolean();
        etFrameTime = (EditText)findViewById(R.id.et_frametime);
        etFrameTime.setText(String.valueOf((int )Settings.ThreadTime));
        etAngle = (EditText)findViewById(R.id.et_angle);
        etSFxavg = (EditText)findViewById(R.id.et_SFxavg);
        etThreadDuration = (EditText)findViewById(R.id.et_threadduration);
        etThreadDuration.setText(String.valueOf((int)Settings.DurationTime));
        Circle circle = (Circle) findViewById(R.id.circle);
        ake_orientation=(TextView)findViewById(R.id.textViewOrientation) ;
        signalProcessing= new SignalProcessing();


        alert = new AlertDialog.Builder(this);
        input = new EditText(this);
        main_context=this;



        enableButtons(false);
        enbaleSettings(false);
        setupListeners();

        Settings.et_degree = etAngle;
        Settings.et_SFxavg = etSFxavg;
        Settings.et_circle = circle;
        Settings.ake_orientation=ake_orientation;

        updateGUI();
    }

    private void enableButtons(boolean flag) {
        ((ImageButton)findViewById(R.id.ib_mic)).setEnabled(!flag);
        ((ImageButton)findViewById(R.id.ib_stop)).setEnabled(flag);
        ((ImageButton)findViewById(R.id.ib_settings)).setEnabled(!flag);
        ((Button)findViewById(R.id.b_resultsaver)).setEnabled(!flag);
    }

    private void setupListeners(){
        ((ImageButton)findViewById(R.id.ib_mic)).setOnClickListener(buttonClick);
        ((ImageButton)findViewById(R.id.ib_stop)).setOnClickListener(buttonClick);
        ((ImageButton)findViewById(R.id.ib_settings)).setOnClickListener(buttonClick);
        ((Button)findViewById(R.id.b_save)).setOnClickListener(buttonClick);
        ((Button)findViewById(R.id.b_reset)).setOnClickListener(buttonClick);
        ((Button)findViewById(R.id.b_resultsaver)).setOnClickListener(buttonClick);
    }

    private void enbaleSettings(boolean flag) {
        ((EditText) findViewById(R.id.et_frametime)).setEnabled(flag);
        ((EditText) findViewById(R.id.et_angle)).setEnabled(flag);
        ((EditText) findViewById(R.id.et_SFxavg)).setEnabled(flag);
        ((EditText) findViewById(R.id.et_threadduration)).setEnabled(flag);
        ((Button) findViewById(R.id.b_save)).setEnabled(flag);
        ((Button) findViewById(R.id.b_reset)).setEnabled(flag);

    }


    private View.OnClickListener buttonClick = new View.OnClickListener(){
        public void onClick(View v){
            switch (v.getId()){
                case R.id.ib_mic:{
                    //tv.setText(stringFromJNI());

                    StartRecord();



                    ////////////////////////
                    //////////////////////
                    Circle circle = (Circle) findViewById(R.id.circle);
                    //CircleView circle=new CircleView(this,45) ;
                    Circleanimation animation = new Circleanimation(circle, Settings.output_angle);
                    //animation.setInterpolator(new BounceInterpolator());
                    animation.setDuration(1000);
                    circle.startAnimation(animation);

                    String output_a = Float.toString(Settings.output_angle);
                    etAngle.setText(output_a);
                    etSFxavg.setText("0");                  ////////////////////////////
                    ///////////////////////////


                    break;
                }
                case R.id.ib_settings:{
                    ((ImageButton)findViewById(R.id.ib_mic)).setEnabled(false);
                    ((ImageButton)findViewById(R.id.ib_stop)).setEnabled(false);
                    ((ImageButton)findViewById(R.id.ib_settings)).setEnabled(false);
                    enbaleSettings(true);
                    break;
                }
                case R.id.ib_stop:{
                    Settings.setPlayback(true);

                    StopRecord();
                    break;
                }
                case R.id.b_save:{
                    enableButtons(false);
                    enbaleSettings(false);
                    updateSettings();
                    break;
                }
                case R.id.b_reset:{
                    resetSettings();
                    break;
                }
                case R.id.b_resultsaver:{
                    if(isResultSaver){
                        ((Button) findViewById(R.id.b_resultsaver)).setBackgroundColor(Color.RED);
                        isResultSaver=false;




                    }else {
                        ((Button) findViewById(R.id.b_resultsaver)).setBackgroundColor(Color.GREEN);
                        isResultSaver=true;

                        // POP-UP for GT angle
                        alert.setTitle("What is the speaker angle?");
                        alert.setMessage("Please enter the times of 20");

                        // Set an EditText view to get user input
                        alert.setView(input);
                        input.setInputType(InputType.TYPE_CLASS_NUMBER);
                        input.getText().clear();



                        alert.setPositiveButton("Ok", new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface dialog, int whichButton) {

                                if (input.getText().toString().trim().length()>0)
                                    Settings.GT_angle=Integer.valueOf(input.getText().toString());
                            }
                        });

                        alert.setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface dialog, int whichButton) {
                                // Canceled.
                            }
                        });

                        if(input.getParent() != null) {
                            ((ViewGroup)input.getParent()).removeView(input);
                        }

                        alert.show();
                    }
                    enchancedDOA();
                    break;
                }
            }
        }
    };

    private void StopRecord() {
        switch(mode){
            case 1:{
                waveRecorder.stopRecording();
                waveRecorder = null;
                enableButtons(false);
                signalProcessing.release();

                if (isResultSaver){
                    LayoutInflater inflater = getLayoutInflater();

                    View toastLayout = inflater.inflate(R.layout.ake_toast,
                            (ViewGroup) findViewById(R.id.toast_root_view));

                    int accuracy=ake_processing.getAccuracy();
                    int player_tr_idx=ake_processing.getPlayertrackidx();
                    if(player_tr_idx==0)
                        player_tr_idx=1;
                    double acc_percentage=(double) ((accuracy*100)/player_tr_idx);
                    double sum_mse =ake_processing.getSumMSE();
                    TextView acc = (TextView) toastLayout.findViewById(R.id.toast_acc_val);
                    acc.setText(String.valueOf(acc_percentage));


                    double rmse_val= Math.sqrt(sum_mse/(double)player_tr_idx);
                    TextView rmse = (TextView) toastLayout.findViewById(R.id.toast_rmse_val);
                    rmse.setText(String.valueOf(rmse_val));

                    final Toast toast = new Toast(getApplicationContext());
                    toast.setGravity(Gravity.CENTER, 0, 0);
                    toast.setDuration(Toast.LENGTH_LONG);
                    toast.setView(toastLayout);
                    toast.show();
                    DecimalFormat REAL_FORMATTER = new DecimalFormat("0.###");

                    Settings.ake_orientation.setText("Accuracy is "+String.valueOf(REAL_FORMATTER.format(acc_percentage))+
                            " percent over "+ String.valueOf(player_tr_idx)+ " estimations"+
                            "\n RMSE: "+String.valueOf(REAL_FORMATTER.format(rmse_val)));

                    StringBuilder file_name_b= new StringBuilder("Estimations_for_");
                    file_name_b.append(Settings.GT_angle);
                    file_name_b.append(".txt");
                    file_name = file_name_b.toString();
                    String[] estimated_angles=ake_processing.getEst_angles();

                    try {
                        File root = new File(Environment.getExternalStorageDirectory(), "Estimated Angles");
                        if (!root.exists()) {
                            root.mkdirs();
                        }
                        gpxfile = new File(root, file_name);
                        FileWriter writer = new FileWriter(gpxfile);
                        for (String s : estimated_angles) {
                            writer.append(s);
                            writer.append("\n");
                        }
                        writer.flush();
                        writer.close();
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                    getApplicationContext().getFilesDir();



                }
                break;
            }
        }
    }



    protected void resetSettings() {
        etFrameTime.setText(R.string.etFrametime);
        Settings.setupFrameTime(Settings.frameTime);
        etAngle.setText(R.string.etStepsize);
        Settings.setupStepSize(Settings.stepSize);
        Settings.setupThreadTime(Settings.ThreadTime);
//        rbEnhanced.setChecked(true);
        // rbOriginal.setChecked(false);
        //rbBabble.setChecked(true);
        ////rbDrivingcar.setChecked(false);
        //rbMachinery.setChecked(false);
        Settings.setupPlayback(1);
        Settings.setupNoiseType(1);
    }

    protected void updateSettings() {
        float initial_angle = 0;
        float initial_frame = 0;

        //Settings.setupStepSize(Float.parseFloat(etAngle.getText().toString()));
        Log.e("MainGUI","StepSize:"+Float.toString(Settings.stepFactor));
        //Settings.setupFrameTime(Float.parseFloat(etFrameTime.getText().toString()));
        Settings.setupThreadTime(Float.parseFloat(etFrameTime.getText().toString()));
        Settings.setupDurationTime(Float.parseFloat(etThreadDuration.getText().toString()));
        Log.e("MainGUI","FrameTime:"+Float.toString(Settings.frameTime));
        //myradiobutton = (RadioButton)findViewById(selectId);
        Settings.setupPlayback(playback);
        Log.e("MainGUI","Playback:"+Integer.toString(Settings.debugLevel));
    }

    private void StartRecord()  {
        if (ActivityCompat.checkSelfPermission(this, Manifest.permission.RECORD_AUDIO) !=
                PackageManager.PERMISSION_GRANTED) {
            //   statusView.setText(getString(R.string.status_record_perm));
            ActivityCompat.requestPermissions(
                    this,
                    new String[] { Manifest.permission.RECORD_AUDIO },
                    AUDIO_ECHO_REQUEST);
            return;
        }

        if (ActivityCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE) !=
                PackageManager.PERMISSION_GRANTED) {
            //   statusView.setText(getString(R.string.status_record_perm));
            ActivityCompat.requestPermissions(
                    this,
                    new String[] { Manifest.permission.WRITE_EXTERNAL_STORAGE },
                    AUDIO_ECHO_REQUEST);
            return;
        }

        if (ActivityCompat.checkSelfPermission(this, Manifest.permission.MODIFY_AUDIO_SETTINGS) !=
                PackageManager.PERMISSION_GRANTED) {
            //   statusView.setText(getString(R.string.status_record_perm));
            ActivityCompat.requestPermissions(
                    this,
                    new String[] { Manifest.permission.MODIFY_AUDIO_SETTINGS },
                    AUDIO_ECHO_REQUEST);
            return;
        }

        inputFrames.clear();
        //inputFrames2.clear();
        processedFrames.clear();
//inputFrames.size();
        ake_processing=new Processing(inputFrames,processedFrames,this);


        waveRecorder = new WaveRecorder(inputFrames);

        if(Settings.debugLevel!=2){
            mode = 1;
            //new AudioSaver(getString(R.string.appDirectory)+ File.separator+System.currentTimeMillis(),processedFrames);
        }
        //Log.e("AudioSaver"," is initialized successfully!");
        //startPlay();

        processingFlag.set(true);
        enableButtons(true);
    }
    private void enchancedDOA() {


        Settings.setupResultSaver(isResultSaver);

    }

    public int getMode() {
        return mode;
    }

    public void done() {
        if(processingFlag.getAndSet(false)) {
            runOnUiThread(
                    new Runnable() {
                        public void run() {
                            enableButtons(false);
                        }
                    }
            );
        }
    }

    public void notify(String message) {

    }
    /*
     * Loading our Libs
     */
    static {
        System.loadLibrary("echo");
    }

    /*
     * jni function implementations...
     */
    //public static native void createSLEngine(int rate, int framesPerBuf);
    /*public static native void deleteSLEngine();

    public static native boolean createSLBufferQueueAudioPlayer();
    public static native void deleteSLBufferQueueAudioPlayer();

    public static native boolean createAudioRecorder();
    public static native void deleteAudioRecorder();
    public static native void startPlay();
    public static native void stopPlay();*/

    public void onResume() {
        super.onResume();




        // setContentView(animation);



    }

    public void updateGUI()
    {
        //CircleView circle=new CircleView(this,45) ;
        Circleanimation animation = new Circleanimation(Settings.et_circle, Settings.output_angle);
        //animation.setInterpolator(new BounceInterpolator());
        //animation.setDuration(1000);
        Settings.et_circle.startAnimation(animation);

        String output_a = Float.toString(Settings.output_angle);
        //EditText etAngle = (EditText)findViewById(R.id.et_angle);;
        //output_a = "!!!";
        //EditText etAngle = (EditText)findViewById(R.id.et_angle);
        Settings.et_degree.setText(output_a);

        String output_ab = Float.toString(Settings.SFx_avg);
        //String output_ab1 = output_ab.substring(0,3);
        //output_ab = String.format("%.4f", output_ab);
        Settings.et_SFxavg.setText(output_ab);
        Settings.ake_orientation.setTextSize(17);
        // Settings.ake_orientation.setText("Re-orient by "+output_a+" degree ");
    }

    public void updateGUI_text()
    {
        float output_degree = 0;
        long tmp = 15;
        if(Settings.angle_counter<2*tmp)
        {
            output_degree = 0;
            Settings.angle_counter++;
        }
        else if(Settings.angle_counter<3*tmp+3)
        {
            output_degree = 15;
            Settings.angle_counter++;
        }
        else if(Settings.angle_counter<4*tmp)
        {
            output_degree = 30;
            Settings.angle_counter++;
        }
        else if(Settings.angle_counter<5*tmp-2)
        {
            output_degree = 45;
            Settings.angle_counter++;
        }
        else if(Settings.angle_counter<6*tmp+3)
        {
            output_degree = 60;
            Settings.angle_counter++;
        }
        else if(Settings.angle_counter<7*tmp)
        {
            output_degree = 75;
            Settings.angle_counter++;
        }
        else if(Settings.angle_counter<8*tmp-2)
        {
            output_degree = 90;
            Settings.angle_counter++;
        }
        else if(Settings.angle_counter<9*tmp)
        {
            output_degree = 105;
            Settings.angle_counter++;
        }
        else if(Settings.angle_counter<10*tmp)
        {
            output_degree = 120;
            Settings.angle_counter++;
        }
        else if(Settings.angle_counter<11*tmp)
        {
            output_degree = 135;
            Settings.angle_counter++;
        }
        else if(Settings.angle_counter<12*tmp)
        {
            output_degree = 150;
            Settings.angle_counter++;
        }
        else if(Settings.angle_counter<13*tmp)
        {
            output_degree = 165;
            Settings.angle_counter++;
        }
        else if(Settings.angle_counter<14*tmp)
        {
            output_degree = 180;
            Settings.angle_counter++;
        }
        else if(Settings.angle_counter<15*tmp-2)
        {
            output_degree = 165;
            Settings.angle_counter++;
        }
        else if(Settings.angle_counter<16*tmp)
        {
            output_degree = 150;
            Settings.angle_counter++;
        }
        else if(Settings.angle_counter<17*tmp)
        {
            output_degree = 135;
            Settings.angle_counter++;
        }
        else if(Settings.angle_counter<18*tmp)
        {
            output_degree = 120;
            Settings.angle_counter++;
        }
        else if(Settings.angle_counter<19*tmp)
        {
            output_degree = 105;
            Settings.angle_counter++;
        }
        else if(Settings.angle_counter<20*tmp)
        {
            output_degree = 90;
            Settings.angle_counter++;
        }
        else if(Settings.angle_counter<21*tmp+6)
        {
            output_degree = 75;
            Settings.angle_counter++;
        }
        else
        {
            Settings.angle_counter = 14*tmp;
        }

        Settings.angle_counter++;

        //CircleView circle=new CircleView(this,45) ;
        Circleanimation animation = new Circleanimation(Settings.et_circle, output_degree);
        //animation.setInterpolator(new BounceInterpolator());
        //animation.setDuration(1000);
        Settings.et_circle.startAnimation(animation);

        String output_a = Float.toString(output_degree);
        //EditText etAngle = (EditText)findViewById(R.id.et_angle);;
        //output_a = "!!!";
        //EditText etAngle = (EditText)findViewById(R.id.et_angle);
        Settings.et_degree.setText(output_a);

        //output_a = Float.toString(Settings.SFx_avg);
        Settings.et_SFxavg.setText(output_a);
    }



}
