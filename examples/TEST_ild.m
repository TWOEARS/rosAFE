%%
clear all; close all; clc;
%%
[ bass, rosAFE ] = initRosAFE();

%% Acquiring Audio
acquireAudio();

%% Ading processors
inputName = 'input';
thisProc = rosAFE.InputProc('-a', inputName, 12000, bufferSize_s_rosAFE_port, bufferSize_s_rosAFE_getSignal );
pause(p);

preProcName = 'preProc';
preProcProc = rosAFE.PreProc('-a', preProcName, inputName, 0, ... % 'pp_bRemoveDC'
                                                           5000, ... % 'pp_cutoffHzDC'
                                                           0, ... % 'pp_bPreEmphasis'
                                                           0.97, ... % 'pp_coefPreEmphasis'
                                                           0, ... % 'pp_bNormalizeRMS'
                                                           500e-3, ... % 'pp_intTimeSecRMS'
                                                           0, ... % 'pp_bLevelScaling'
                                                           10, ... % 'pp_refSPLdB'
                                                           0, ... % 'pp_bMiddleEarFiltering'
                                                           'jespen', ... % 'pp_middleEarModel'
                                                           1 ); % 'pp_bUnityComp'
pause(p);

gammatoneName = 'gammatone';
gammatoneProc = rosAFE.GammatoneProc('-a', gammatoneName, preProcName, 'gammatone', ... % 'fb_type'
                                                                       80, ... % 'fb_lowFreqHz'
                                                                       8000, ... % 'fb_highFreqHz'
                                                                       1, ... % 'fb_nERBs'
                                                                       23, ... % 'fb_nChannels'
                                                                       0, ... % 'fb_cfHz'
                                                                       4, ... % 'fb_nGamma'
                                                                       1.0180 );  % 'fb_bwERBs'
pause(p);

ihcName = 'ihc';
ihcProc = rosAFE.IhcProc('-a', ihcName, gammatoneName, 'dau' );  % ihc_method
pause(p);

ildName = 'ild';
ildProc = rosAFE.IldProc('-a', ildName, ihcName, 'hann', ... % ild_wname
                                                  0.02, ... % ild_wSizeSec
                                                  0.01 );  % ild_hSizeSec
pause(p);

%%
exLastFrameIndex = 0;

% Getting the parameters
params = rosAFE.getParameters();
if ( strcmp(params.status,'error') )
   error(strcat('Error',params.exception.ex));
end

cfHz = cell2mat(params.result.parameters.gammatone{1}.fb_cfHz);

ildOut = rosAFE.ildPort(ildName);
exLastFrameIndex = ildOut.ildPort.lastFrameIndex;

sig_m = TimeFrequencySignal.construct(ildOut.ildPort.sampleRate, bufferSize_s_matlab, 'ild', ildName, cfHz,'mono');

while( 1 )
    ildOut = rosAFE.ildPort(ildName);
    gotFrames = ( ildOut.ildPort.lastFrameIndex - exLastFrameIndex ) / sampleRate * ildOut.ildPort.sampleRate;
    exLastFrameIndex = ildOut.ildPort.lastFrameIndex;
    if ( gotFrames > ildOut.ildPort.framesOnPort )
        lostFrames = gotFrames - ildOut.ildPort.framesOnPort;
        gotFrames = gotFrames - lostFrames;
        disp(strcat('Lost Frames : ', int2str(lostFrames)));
    else
        lostFrames = 0;
    end

    [ chunkLeft ] = adaptTFS( ildOut.ildPort.framesOnPort, ...
                                         ildOut.ildPort.numberOfChannels, ...
                                         ildOut.ildPort.left, ...
                                         false );

    sig_m.appendChunk( chunkLeft(end-floor(gotFrames)+1:end,:) );

    f = figure(1);
    sig_m.plot(f);
    pause(0.1);
end

stopAndKill();