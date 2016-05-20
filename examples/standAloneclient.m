clear all;
close all;
clc;
p=0.25;

%% Paths
addpath(genpath('~/openrobots/lib/matlab'));

%% Genom
client = genomix.client;
pause(p);
bass = client.load('bass');
pause(p);
rosAFE = client.load('rosAFE');
pause(p);

%% Acquiring Audio
sampleRate = 44100;
bufferSize_s_bass = 1;
bufferSize_s_rosAFE = 10;
nFramesPerChunk = 2205;
nChunksOnPort = sampleRate * bufferSize_s_bass / nFramesPerChunk;
inputDevice = 'hw:2,0';

% acquire = bass.Acquire('-a', inputDevice, sampleRate, nFramesPerChunk, nChunksOnPort);
% pause(0.25);
% if ( strcmp(acquire.status,'error') )
%    error(strcat('Error',acquire.exception.ex));
% end
menu('Launch rosbag now','Done');

%% Connecting rosAFE to BASS
connection = rosAFE.connect_port('Audio', 'bass/Audio');
pause(p);
if ( strcmp(connection.status,'error') )
    error(strcat('Error',connection.exception.ex));
end    
            
%% Ading a request
inputName = 'input';
thisProc = rosAFE.InputProc('-a', inputName, 12000, bufferSize_s_rosAFE );
pause(p);

preProcName = 'preProc2';
preProcProc = rosAFE.PreProc('-a', preProcName, inputName, sampleRate, 0, ... % 'pp_bRemoveDC'
                                                                       20, ... % 'pp_cutoffHzDC'
                                                                       1, ... % 'pp_bPreEmphasis'
                                                                       0.97, ... % 'pp_coefPreEmphasis'
                                                                       0, ... % 'pp_bNormalizeRMS'
                                                                       1, ... % 'pp_bBinauralRMS'
                                                                       500e-3, ... % 'pp_intTimeSecRMS'
                                                                       0, ... % 'pp_bLevelScaling'
                                                                       100, ... % 'pp_refSPLdB'
                                                                       0, ... % 'pp_bMiddleEarFiltering'
                                                                       'jespen', ... % 'pp_middleEarModel'
                                                                       1 ); % 'pp_bUnityComp'

%% Getting the output

preProc = rosAFE.preProcPort(preProcName);
inputPort = rosAFE.inputProcPort();
bassPort = bass.Audio();

%% Tools

disp(strcat('Bass last Frame : ',int2str(bassPort.Audio.lastFrameIndex),' . Input last frame : ',int2str(inputPort.inputProcPort.lastFrameIndex),' . preProc last frame : ', int2str(preProc.preProcPort.lastFrameIndex)));

sound = [cell2mat(inputPort.inputProcPort.left.data)' cell2mat(inputPort.inputProcPort.right.data)'];
audiowrite('input.wav',sound, sampleRate);

sound = [cell2mat(preProc.preProcPort.left.data)' cell2mat(preProc.preProcPort.right.data)'];
audiowrite('pp.wav',sound, sampleRate);
% audiowrite('preProc.wav',[dObj.time_input_0_0{1}.Data(:) dObj.time_input_0_0{2}.Data(:)], sampleRate);




