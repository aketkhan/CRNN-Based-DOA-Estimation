//
// Created by axk166230 on 9/11/2017.
//


#include <cassert>
#include <cstring>
#include <jni.h>

#include <sys/types.h>
#include <SLES/OpenSLES.h>

//#include "audio_common.h"
//#include "audio_recorder.h"
//#include "audio_player.h"
#include "audio_main.h"
double ake_sum=0;
double ake_timing=0;
int xx=0;
/*struct EchoAudioEngine {
    SLmilliHertz fastPathSampleRate_;
    uint32_t     fastPathFramesPerBuf_;
    uint16_t     sampleChannels_;
    uint16_t     bitsPerSample_;

    SLObjectItf  slEngineObj_;
    SLEngineItf  slEngineItf_;

    //AudioRecorder  *recorder_;
    //AudioPlayer    *player_;
    //AudioQueue     *freeBufQueue_;    //Owner of the queue
    //AudioQueue     *recBufQueue_;     //Owner of the queue

    //sample_buf  *bufs_;
    uint32_t     bufCount_;
    uint32_t     frameCount_;
};*/
//static EchoAudioEngine engine;


//bool EngineService(void* ctx, uint32_t msg, void* data );

extern "C" {
//JNIEXPORT void JNICALL
//Java_com_google_sample_echo_MainActivity_createSLEngine(JNIEnv *env, jclass, jint, jint);
JNIEXPORT jlong JNICALL
Java_com_example_doa_1crnn_1android_1app_SignalProcessing_paramInitilization(JNIEnv *env, jobject thiz,int frequency, int stepSize,int frameSize, float stepFactor, int noisetype, float ThreadTime, float DurationTime, int Image_H, int Image_W );
JNIEXPORT void JNICALL
Java_com_example_doa_1crnn_1android_1app_SignalProcessing_realtimeProcessing(JNIEnv *env, jobject thiz,  jlong memoryPointer, jshortArray input);
JNIEXPORT void JNICALL
Java_com_example_doa_1crnn_1android_1app_SignalProcessing_paramElimination(JNIEnv* env, jobject thiz, jlong memoryPointer);
JNIEXPORT jshortArray JNICALL
Java_com_example_doa_1crnn_1android_1app_SignalProcessing_soundOutput(JNIEnv* env, jobject thiz, jlong memoryPointer, jint outputSelection);

JNIEXPORT jfloatArray JNICALL
Java_com_example_doa_1crnn_1android_1app_SignalProcessing_GetCNNInput(JNIEnv *env, jclass type, jlong memoryPointer);

JNIEXPORT jshortArray JNICALL
Java_com_example_doa_1crnn_1android_1app_SignalProcessing_dataOutput(JNIEnv* env, jobject thiz, jlong memoryPointer, jint outputSelection);
JNIEXPORT jfloat JNICALL
Java_com_example_doa_1crnn_1android_1app_SignalProcessing_getTime(JNIEnv* env, jobject thiz, jlong memoryPointer);
JNIEXPORT jfloat JNICALL
Java_com_example_doa_1crnn_1android_1app_SignalProcessing_getComputeTime(JNIEnv* env, jobject thiz, jlong memoryPointer);
JNIEXPORT jfloat JNICALL
Java_com_example_doa_1crnn_1android_1app_SignalProcessing_getFilteringTime(JNIEnv* env, jobject thiz, jlong memoryPointer);


JNIEXPORT jintArray JNICALL
Java_com_example_doa_1crnn_1android_1app_SignalProcessing_Getframecounter(JNIEnv *env, jclass type,
                                                                          jlong memoryPointer);}









JNIEXPORT jlong JNICALL
Java_com_example_doa_1crnn_1android_1app_SignalProcessing_paramInitilization(
        JNIEnv *env, jobject thiz,int frequency, int stepSize,int frameSize, float stepFactor, int noisetype, float ThreadTime, float DurationTime, int Image_H, int Image_W){
    Variables *mainParam = (Variables*)malloc(sizeof(Variables));

    //int i;
   // mainParam->counter = 0;
   // mainParam->u = stepFactor;
    mainParam->stepSize = stepSize;
    mainParam->samplingRate = frequency;
    mainParam->frameSize = frameSize;  ////Threadshold Time
    mainParam->filLen = 1;
    mainParam->topMicBuffer = (float*)calloc(frameSize+mainParam->filLen-1,sizeof(float));
    mainParam->botMicBuffer = (float*)calloc(frameSize+mainParam->filLen-1,sizeof(float));

    mainParam->x_frame = (float*)malloc(stepSize * sizeof(float));
    mainParam->h_frame = (float*)malloc(stepSize * sizeof(float));

    mainParam->originalBuffer = (int*)calloc(2*stepSize,sizeof(int));
    mainParam->mixedBuffer = (short*)calloc(stepSize,sizeof(short));
    /*mainParam->w = (float*)malloc(mainParam->filLen*sizeof(float));
    for(i=0;i<mainParam->filLen;i++){
        mainParam->w[i]=1;
    }
    mainParam->y_curr = (float*)calloc(frameSize,sizeof(float));
    mainParam->y_prev = (float*)calloc(frameSize,sizeof(float));
    mainParam->y = (float*)calloc(stepSize,sizeof(float));
    mainParam->e = (float*)calloc(stepSize,sizeof(float));//*/
   // mainParam->outputBuffer = (float*)calloc(stepSize,sizeof(float));
   // mainParam->uplimit = 5*frequency/stepSize;
    //mainParam->logmmsePtr = initiallogMMSE(frameSize, stepSize, frequency,noisetype);
    //mainParam->timer = newTimer();
    mainParam->Trainingframe = (int)ThreadTime;
    mainParam->DurationThresh = (int)DurationTime;

    mainParam->angle = 0;
    mainParam->speech_count = 0;
   // mainParam->twoDOA = newDOA(DOA_NFFTT, stepSize, DOA_fss,ThreadTime,DurationTime);
   // mainParam->isEnchanced=isEnchanced;
   // mainParam->logMMSE=newparameters(DOA_NFFTT, stepSize, 50); //PERC=50
    //mainParam->logMMSE2=newparameters(DOA_NFFTT, DOA_nn, 50); //PERC=50
    mainParam->ake_counter=0;
    mainParam->ake_avg_timer=0;

    /*Memory Update*/
    mainParam->_output=(float*)calloc(3,sizeof(float)); // We have 3 output

    mainParam->trans1= newTransform(DOA_NFFTT);
    mainParam->trans2= newTransform(DOA_NFFTT);
    mainParam->DOA_CNN_input= (float *)calloc(4*(DOA_NFFTT/2+1),sizeof(float));
    //mainParam->DOA_CNN_input [5][Image_W*4];
    mainParam->frcount= (int *)malloc( sizeof(int));
    mainParam->image_h=Image_H;
    mainParam->image_w=Image_W;
    mainParam->wind = (float*)malloc((DOA_nn) * sizeof(float));
    mainParam->wind=Hamming(DOA_nn);
    mainParam->currmag_fft_Framex =(float*)malloc((DOA_NFFTT) * sizeof(float));
    mainParam->prevmag_fft_Framex =(float*)malloc((DOA_NFFTT) * sizeof(float));
    mainParam->sfx =(float*)malloc((DOA_NFFTT) * sizeof(float));
    mainParam->isSpeech=false;
    mainParam->sum_sfx=0;
    mainParam->SFx =0;
    //mainParam->VADarray=(jboolean *)malloc( sizeof(jboolean));

    __android_log_print(ANDROID_LOG_ERROR, "Parameter Initialized","Successfully");
    return (jlong)mainParam;
}


JNIEXPORT void JNICALL
Java_com_example_doa_1crnn_1android_1app_SignalProcessing_realtimeProcessing(JNIEnv *env, jobject thiz,  jlong memoryPointer, jshortArray input){
    Variables *mainParam = (Variables*) memoryPointer;
    //startTimer(mainParam->timer);
    mainParam->_in = env->GetShortArrayElements(input, NULL);
    int i,j;//stepSize,frameSize,filLen;
    //float  mThreadTime, mDurationTime;//tempVar,tempEng,
    /*stepSize = mainParam->stepSize;
    frameSize = mainParam->frameSize;
    filLen = mainParam->filLen;//
    mThreadTime = mainParam->Threadtime;
    mDurationTime = mainParam->DurationTime;//*/


    //double * x_frame = (double*)malloc(DOA_nn * sizeof(double));
    //double * h_frame = (double*)malloc(DOA_nn * sizeof(double));

    for (int k = 0; k < DOA_nn; k++)
    {

        mainParam-> x_frame[k] = mainParam->botMicBuffer[k] * mainParam->wind[k];
        mainParam-> h_frame[k] = mainParam->topMicBuffer[k] * mainParam->wind[k];

    }
   //*
    clock_t t;
    t = clock();


    //mainParam->trans2->doTransform(mainParam->trans2,mainParam->x_frame);
    //mainParam->trans1->doTransform(mainParam->trans1,mainParam->h_frame);
    FFT(mainParam->trans2,mainParam->h_frame);
    FFT(mainParam->trans1,mainParam->x_frame);
    t = clock() - t;


    mainParam->sum_sfx=0;
    mainParam->ake_counter= mainParam->ake_counter+1;

    ake_timing=(((double)t)/ CLOCKS_PER_SEC)+(mainParam->ake_avg_timer*(mainParam->ake_counter));
    mainParam->ake_avg_timer=ake_timing/(mainParam->ake_counter);

    __android_log_print(ANDROID_LOG_INFO,"Time", "Time1= %1.9g",((float)mainParam->ake_avg_timer));


    for (int i = 0; i < DOA_NFFTT/2+1; i++)
    {

        mainParam->currmag_fft_Framex[i] = sqrt(pow(mainParam->trans1->real[i],2)+ pow(mainParam->trans1->imaginary[i], 2));
        //mainParam->currmag_fft_Frameh[i] = sqrt(pow(mainParam->trans2->real[i], 2) + pow(mainParam->trans2->imaginary[i], 2));


        mainParam->sfx[i] = mainParam->currmag_fft_Framex[i] - mainParam->prevmag_fft_Framex[i];

        mainParam->sum_sfx += pow( mainParam->sfx[i],2);
        //sum_sfh += pow( twoDOA->sfh[i], 2);

        //printf("mag_fft_Framex_real=%1.7g\t i=%d\n", mag_fft_Frameh[i], i);

        mainParam->prevmag_fft_Framex[i] = mainParam->currmag_fft_Framex[i];
       // twoDOA->prevmag_fft_Frameh[i] = twoDOA->currmag_fft_Frameh[i];

    }

    // double SFx;// SFh;
    mainParam->SFx = mainParam->sum_sfx / (DOA_NFFTT);


    if (mainParam->ake_counter==0) {
        mainParam->SFxavg = mainParam->SFx;
        mainParam->SFxmax = mainParam->SFx;
    }
    else {

        if (mainParam->ake_counter<=mainParam->Trainingframe) {
            if (mainParam->SFx > mainParam->SFxmax) {

                mainParam->SFxmax = mainParam->SFx;

            }
            mainParam->SFxavg = (mainParam->SFxavg*(mainParam->ake_counter-1 ) + mainParam->SFx) / mainParam->ake_counter;
        }
    }

    if ((mainParam->ake_counter >= mainParam->Trainingframe) && (mainParam->SFx > mainParam->SFxavg) && (mainParam->SFx > mainParam->SFxmax)) {

        mainParam->flagSFx +=   1;										/*   % Duration*/
    }
    else {
        mainParam->flagSFx = 0;
    }


    if (mainParam->ake_counter >= mainParam->Trainingframe) {
        if (mainParam->flagSFx > mainParam->DurationThresh){
            mainParam->isSpeech=true;
            mainParam->speech_count++;

        }


        else
            mainParam->isSpeech=false;


    }
    else {

        mainParam->isSpeech=false;
    }
   /*  if (xx==51){
     FILE *file1 = fopen("/sdcard/mich.txt", "w+");
     for (int i = 0; i < DOA_nn; ++i) {


         fprintf(file1, "%1.9g\n", mainParam->h_frame[i]);


     }

     fflush(file1);
     fclose(file1);

 }


     if (xx==51){
     FILE *file2 = fopen("/sdcard/mich_fft.txt", "w+");
     for (int i = 0; i < DOA_NFFTT/2+1; ++i) {


         fprintf(file2, "%1.9g\t%1.9g\n", mainParam->trans2->real[i], mainParam->trans2->imaginary[i]);


     }

     fflush(file2);
     fclose(file2);

 }
 xx++;*/

    /*t = clock() - t;

    ake_timing=(((double)t)/ CLOCKS_PER_SEC)+(mainParam->ake_avg_timer*(mainParam->ake_counter));

    mainParam->ake_avg_timer=ake_timing/(mainParam->ake_counter);//*/

     /* if (mainParam->angle_count %20 == 0){
    __android_log_print(ANDROID_LOG_INFO,"Time", "Time1= %1.9g",((float)mainParam->ake_avg_timer));
          }*/
    //__android_log_print(ANDROID_LOG_INFO,"Time", "Time1= %1.9g",(((double)t)/ CLOCKS_PER_SEC));


    /*if(mainParam->angle_count==100){

        tc=clock();
    }
*/
    /* FILE *file1 = fopen("/sdcard/mic.txt", "w+");

     fprintf(file1, "%d\t%ld click\t %1.9g\n", mainParam->angle_count, t,((float)t)/ CLOCKS_PER_SEC);
     fflush(file1);
     fclose(file1);*/



    //__android_log_print(ANDROID_LOG_INFO,"Angle", "The angle = %lf",mainParam->twoDOA->corrtheta_est);
    //__android_log_print(ANDROID_LOG_INFO,"Time", "Time= %1.9g",((float)t)/ CLOCKS_PER_SEC);

    for(i = 0;i<mainParam->filLen-1;i++){
        mainParam->topMicBuffer[i] = mainParam->topMicBuffer[i+mainParam->stepSize];
        mainParam->botMicBuffer[i] = mainParam->botMicBuffer[i+mainParam->stepSize];
    }
    for(i = mainParam->filLen-1,j=0;i<mainParam->filLen+mainParam->stepSize-1;i++,j+=2){
        mainParam->topMicBuffer[i]= mainParam->topMicBuffer[i+mainParam->stepSize];
        mainParam->topMicBuffer[i+mainParam->stepSize]= mainParam->_in[j+1]*FAC;
        //mainParam->topMicBuffer[i+stepSize]= _in[2*j]*FAC;
        //mainParam->topMicBuffer[i+stepSize+1]= _in[2*j+1]*FAC;
        mainParam->botMicBuffer[i]= mainParam->botMicBuffer[i+mainParam->stepSize];
        mainParam->botMicBuffer[i+mainParam->stepSize]= mainParam->_in[j]*FAC;
        // mainParam->botMicBuffer[i+stepSize]= _in[2*j+2]*FAC;
        //mainParam->botMicBuffer[i+stepSize+1]= _in[2*j+3]*FAC;
        mainParam->originalBuffer[j] = mainParam->_in[j];
        mainParam->originalBuffer[j+1] = mainParam->_in[j+1];
    }
    for(i=0;i<mainParam->stepSize;i++){
        mainParam->mixedBuffer[i] = (mainParam->_in[i*2]+mainParam->_in[i*2+1])/2;
    }
    //__android_log_print(ANDROID_LOG_ERROR, "Parameter Computed 1st","Successfully");
    env->ReleaseShortArrayElements(input, mainParam->_in, 0);
    //tempEng = 0;

    // *************************************************************************************************** //

    // *************************************************************************************************** //
   // startfilteringTimer(mainParam->timer);
   /* for(i=0;i<frameSize;i++){
        mainParam->y_prev[i] = mainParam->y_curr[i];
        tempVar = 0;
        for(j=0;j<filLen;j++){
            tempVar += mainParam->w[j]*mainParam->botMicBuffer[i+j];
            tempEng += mainParam->botMicBuffer[i+j]*mainParam->botMicBuffer[i+j];
        }
        mainParam->y_curr[i] = tempVar;
    }
*/
/*
    for(i=0;i<stepSize;i++){
        mainParam->e[i] = mainParam->topMicBuffer[i+filLen-1]-0.5*(mainParam->y_prev[i+stepSize]+mainParam->y_curr[i]);
    }
    //__android_log_print(ANDROID_LOG_ERROR, "Parameter Computed 2nd","Successfully");
    for(i=0;i<filLen;i++){
        tempVar = 0;
        for(j=0;j<stepSize;j++){
            tempVar += mainParam->botMicBuffer[filLen-1+j]*mainParam->e[j];
            tempVar += mainParam->botMicBuffer[j+i]*mainParam->e[j];
        }
        mainParam->w[i] += mainParam->u*tempVar/(tempEng+EPS);
    }*/
   // stopfilteringTimer(mainParam->timer);
    // *************************************************************************************************** //

    // *************************************************************************************************** //

  /*  if(mainParam->counter<mainParam->uplimit){
        mainParam->counter += 1;
        for(i = 0;i<stepSize;i++){
            mainParam->outputBuffer[i] = mainParam->e[i];
        }
    }else{
        //startcomputeTimer(mainParam->timer);
        //logMMSEexe(mainParam->logmmsePtr, mainParam->e, mainParam->outputBuffer);
       // stopcomputeTimer(mainParam->timer);
    }*/

    // *************************************************************************************************** //

    // *************************************************************************************************** //
    //stopTimer(mainParam->timer);
}


JNIEXPORT void JNICALL
Java_com_example_doa_1crnn_1android_1app_SignalProcessing_paramElimination(JNIEnv* env, jobject thiz, jlong memoryPointer){
    Variables *mainParam = (Variables*) memoryPointer;
    if(mainParam != NULL){
        /*
        if(mainParam->topMicBuffer!= NULL)
        free(mainParam->topMicBuffer);mainParam->topMicBuffer = NULL;
        if(mainParam->botMicBuffer!= NULL)
        free(mainParam->botMicBuffer);mainParam->botMicBuffer = NULL;
        //free(mainParam->outputBuffer);mainParam->outputBuffer = NULL;
        if(mainParam->x_frame!= NULL)
        free(mainParam->x_frame);mainParam->x_frame = NULL;
        if(mainParam->h_frame!= NULL)
        free(mainParam->h_frame);mainParam->h_frame = NULL;
        if(mainParam->originalBuffer!= NULL)
        free(mainParam->originalBuffer);mainParam->originalBuffer = NULL;
        if(mainParam->mixedBuffer!= NULL)
        free(mainParam->mixedBuffer);mainParam->mixedBuffer = NULL;
        if(mainParam->logMMSE!= NULL)
        free(mainParam->logMMSE);mainParam->logMMSE = NULL;
        if(mainParam->_output!= NULL)
        free(mainParam->_output);mainParam->_output = NULL;
        if(mainParam->_in!= NULL)
        free(mainParam->_in);mainParam->_in = NULL;
        //if(mainParam->twoDOA!= NULL)
        //free(mainParam->twoDOA);mainParam->twoDOA = NULL;
        //*/

        /*
        free(mainParam->y_prev);mainParam->y_prev = NULL;
        free(mainParam->y_curr);mainParam->y_curr = NULL;
        free(mainParam->y);mainParam->y = NULL;
        free(mainParam->e);mainParam->e = NULL;
        free(mainParam->w);mainParam->w = NULL;//*/

        //destroylogMMSE(&(mainParam->logmmsePtr));
        //destroyTimer(&(mainParam->timer));
        //free(mainParam);mainParam = NULL;
    }
}

JNIEXPORT jshortArray JNICALL
Java_com_example_doa_1crnn_1android_1app_SignalProcessing_soundOutput(JNIEnv* env, jobject thiz, jlong memoryPointer, jint outputSelection){
    Variables* mainParam = (Variables*) memoryPointer;
    jshortArray output = env->NewShortArray(mainParam->stepSize);
    short *_output = env->GetShortArrayElements( output, NULL);
    int i;
    switch (outputSelection){
        case 0:		// Original

            for (i=0;i<mainParam->stepSize;i++){
                _output[i] = (short)checkRange(mainParam->mixedBuffer[i]);
            }
            //mainParam->angle = mainParam->angle_count;
            //mainParam->angle_count++;

            for (i=0;i<mainParam->stepSize;i++){
                _output[i] = mainParam->mixedBuffer[i];
            }
            break;
    /*    case 1:		// Enhanced
            for (i=0;i<mainParam->stepSize;i++){
                _output[i] = (short)checkRange(mainParam->outputBuffer[i]);
            }

            for (i=0,j=0;i<2*mainParam->stepSize;i+=2,j++){
                _output[i] = (short)checkRange(mainParam->e[j]);
                _output[i+1] = (short)checkRange(mainParam->e[j]);
            }
            break;
        case 2:
            break;//*/
    }
    env->ReleaseShortArrayElements(output, _output, 0);
    return output;
}



JNIEXPORT jfloatArray JNICALL
Java_com_example_doa_1crnn_1android_1app_SignalProcessing_GetCNNInput(JNIEnv *env, jclass type,
                                                                      jlong memoryPointer) {
    Variables* mainParam = (Variables*) memoryPointer;
    jfloatArray output = env->NewFloatArray((DOA_NFFTT)*2+4);
    //float tmp = 0;
    mainParam->DOA_CNN_input = env->GetFloatArrayElements( output, NULL);
    //int i;


    memcpy(mainParam->DOA_CNN_input, mainParam->trans1->imaginary , sizeof(float)*((DOA_NFFTT/2)+1));
    memcpy(mainParam->DOA_CNN_input+((DOA_NFFTT/2)+1), mainParam->trans2->imaginary , sizeof(float)*((DOA_NFFTT/2)+1));
    memcpy(mainParam->DOA_CNN_input+2*((DOA_NFFTT/2)+1), mainParam->trans1->real , sizeof(float)*((DOA_NFFTT/2)+1));
    memcpy(mainParam->DOA_CNN_input+3*((DOA_NFFTT/2)+1), mainParam->trans2->real , sizeof(float)*((DOA_NFFTT/2)+1));




 /*   __android_log_print(ANDROID_LOG_INFO,"X", "x= %d",xx);
    if (xx==51){
        FILE *file1 = fopen("/sdcard/mic.txt", "w+");
        for (int i = 0; i < 2052; ++i) {


            fprintf(file1, "%1.9g\n", mainParam->DOA_CNN_input[i]);


        }

        fflush(file1);
        fclose(file1);

    }
    xx++;
*/



    env->ReleaseFloatArrayElements(output, mainParam->DOA_CNN_input, 0);
//    free(_output);
  //  _output=NULL;
    return output;

}
extern "C"
JNIEXPORT jintArray JNICALL
Java_com_example_doa_1crnn_1android_1app_SignalProcessing_Getframecounter(JNIEnv *env, jclass type,
                                                                     jlong memoryPointer) {
    Variables* mainParam = (Variables*) memoryPointer;
    jintArray output = env->NewIntArray(1);
    //float tmp = 0;
    mainParam->frcount = env->GetIntArrayElements( output, NULL);
    //int i;

    mainParam->frcount[0]=mainParam->speech_count;



    env->ReleaseIntArrayElements(output, mainParam->frcount, 0);
//    free(_output);
    //  _output=NULL;
    return output;

}

extern "C"
JNIEXPORT jbooleanArray JNICALL
Java_com_example_doa_1crnn_1android_1app_SignalProcessing_getVADdecision(JNIEnv *env, jclass type,
                                                                           jlong memoryPointer) {

    Variables* mainParam = (Variables*) memoryPointer;
    jbooleanArray output = env->NewBooleanArray(1);
    mainParam->VADarray =  env->GetBooleanArrayElements(output, NULL);
    mainParam->VADarray[0] = mainParam->isSpeech ? JNI_TRUE : JNI_FALSE;
    env->ReleaseBooleanArrayElements(output,mainParam->VADarray,0);
    return output;

}
JNIEXPORT jshortArray JNICALL
Java_com_example_doa_1crnn_1android_1app_SignalProcessing_dataOutput(JNIEnv* env, jobject thiz, jlong memoryPointer, jint outputSelection){
    Variables* mainParam = (Variables*) memoryPointer;
    jshortArray output = env->NewShortArray(2*mainParam->stepSize);
    short *_output = env->GetShortArrayElements(output, NULL);
    int i;
    switch (outputSelection){
        case 0:
            for(i= 0;i<2*mainParam->stepSize;i++){
                _output[i] = mainParam->originalBuffer[i];
            }
            break;
      /*  case 1:
            for(i= 0;i<2*mainParam->stepSize;i++){
                _output[i] = (short)checkRange(mainParam->outputBuffer[i]);
            }
            break;
        case 2:
            break;//*/
    }
    env->ReleaseShortArrayElements(output, _output, 0);
    return output;
}

/*

JNIEXPORT jfloat JNICALL
Java_com_google_sample_echo_SignalProcessing_getTime(JNIEnv* env, jobject thiz, jlong memoryPointer)
{
    Variables* mainParam = (Variables*) memoryPointer;
    return getTimerMS(mainParam->timer);
}

JNIEXPORT jfloat JNICALL
Java_com_google_sample_echo_SignalProcessing_getComputeTime(JNIEnv* env, jobject thiz, jlong memoryPointer)
{
    Variables* mainParam = (Variables*) memoryPointer;
    return getCT(mainParam->timer);
}
JNIEXPORT jfloat JNICALL
Java_com_google_sample_echo_SignalProcessing_getFilteringTime(JNIEnv* env, jobject thiz, jlong memoryPointer)
{
    Variables* mainParam = (Variables*) memoryPointer;
    return getFT(mainParam->timer);
}
*/


float *Hamming( int np) {// Hamming definition is changed according to Hamming.m file
    float *y = (float*)calloc(np, sizeof(float));
    int a;
    for (a = 0; a < np; a++) {
        y[a] = (0.54 - 0.46*cos((2 * M_PI*a) / (np - 1)));
        //printf("y[%d]=%.17f\n", a, y[a]);

    }

    return y;
    free(y);
    y=NULL;
}


int checkRange(float input)
{
    int output;
    if(input>1){
        output = 32767;
    }else if(input<-1){
        output = -32768;
    }else
        output = 32768*input;
    return output;
}
//////////////
///////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*JNIEXPORT jboolean JNICALL
Java_com_google_sample_echo_MainActivity_createSLBufferQueueAudioPlayer(JNIEnv *env, jclass type) {
    SampleFormat sampleFormat;
    memset(&sampleFormat, 0, sizeof(sampleFormat));
    sampleFormat.pcmFormat_ = (uint16_t)engine.bitsPerSample_;
    sampleFormat.framesPerBuf_ = engine.fastPathFramesPerBuf_;

    // SampleFormat.representation_ = SL_ANDROID_PCM_REPRESENTATION_SIGNED_INT;
    sampleFormat.channels_ = (uint16_t)engine.sampleChannels_;
    sampleFormat.sampleRate_ = engine.fastPathSampleRate_;

    engine.player_ = new AudioPlayer(&sampleFormat, engine.slEngineItf_);
    assert(engine.player_);
    if(engine.player_ == nullptr)
        return JNI_FALSE;

    engine.player_->SetBufQueue(engine.recBufQueue_, engine.freeBufQueue_);
    engine.player_->RegisterCallback(EngineService, (void*)&engine);

    return JNI_TRUE;
}

JNIEXPORT void JNICALL
Java_com_google_sample_echo_MainActivity_deleteSLBufferQueueAudioPlayer(JNIEnv *env, jclass type) {
    if(engine.player_) {
        delete engine.player_;
        engine.player_= nullptr;
    }
}



JNIEXPORT void JNICALL
Java_com_google_sample_echo_MainActivity_deleteAudioRecorder(JNIEnv *env, jclass type) {
    if(engine.recorder_)
        delete engine.recorder_;

    engine.recorder_ = nullptr;
}

JNIEXPORT void JNICALL
Java_com_google_sample_echo_MainActivity_startPlay(JNIEnv *env, jclass type) {

    engine.frameCount_  = 0;*/
    /*
     * start player: make it into waitForData state
     */
 /*
    if(SL_BOOLEAN_FALSE == engine.player_->Start()){
        LOGE("====%s failed", __FUNCTION__);
        return;
    }
    engine.recorder_->Start();
}

JNIEXPORT void JNICALL
Java_com_google_sample_echo_MainActivity_stopPlay(JNIEnv *env, jclass type) {
    engine.recorder_->Stop();
    engine.player_ ->Stop();

    delete engine.recorder_;
    delete engine.player_;
    engine.recorder_ = NULL;
    engine.player_ = NULL;
}



uint32_t dbgEngineGetBufCount(void) {
    uint32_t count = engine.player_->dbgGetDevBufCount();
    count += engine.recorder_->dbgGetDevBufCount();
    count += engine.freeBufQueue_->size();
    count += engine.recBufQueue_->size();

    LOGE("Buf Disrtibutions: PlayerDev=%d, RecDev=%d, FreeQ=%d, "
                 "RecQ=%d",
         engine.player_->dbgGetDevBufCount(),
         engine.recorder_->dbgGetDevBufCount(),
         engine.freeBufQueue_->size(),
         engine.recBufQueue_->size());
    if(count != engine.bufCount_) {
        LOGE("====Lost Bufs among the queue(supposed = %d, found = %d)",
             BUF_COUNT, count);
    }
    return count;
}*/

/*
 * simple message passing for player/recorder to communicate with engine
 */

 /*
bool EngineService(void* ctx, uint32_t msg, void* data ) {
    assert(ctx == &engine);
    switch (msg) {
        case ENGINE_SERVICE_MSG_KICKSTART_PLAYER:
            engine.player_->PlayAudioBuffers(PLAY_KICKSTART_BUFFER_COUNT);
            // we only allow it to call once, so tell caller do not call
            // anymore
            return false;
        case ENGINE_SERVICE_MSG_RETRIEVE_DUMP_BUFS:
            *(static_cast<uint32_t*>(data)) = dbgEngineGetBufCount();
            break;
        default:
            assert(false);
            return false;
    }

    return true;
}
*/


