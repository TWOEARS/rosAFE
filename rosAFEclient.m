addpath(genpath('~/openrobots/lib/matlab'));
addpath(genpath('~/matlab_ws'));

client = genomix.client;
bass = client.load('bass');
rosAFE = client.load('rosAFE');
rosAFE.connect_port('Audio', 'bass/Audio');

sampleRate = 44100;
nFramesPerChunk = 2205;
nChunksOnPort = 20;
bufferSize_s = 1;

p=0.25;
acquire = bass.Acquire('-a', 'hw:1,0', sampleRate, nFramesPerChunk, nChunksOnPort);
pause(p);

get = rosAFE.InputProc('-a', 'input', 12000, bufferSize_s);
pause(p);

preProc = rosAFE.PreProc('-a', 'preProc', 'input', 44100);
pause(p);

preProc1 = rosAFE.PreProc('-a', 'preProc1', 'input', 44100);
pause(p);

gammatoneProc = rosAFE.GammatoneProc('-a', 'gammatone', 'preProc', 44100);
pause(p);

gammatoneProc2 = rosAFE.GammatoneProc('-a', 'gammatone2', 'preProc', 44100);
pause(p);

gammatoneProc3 = rosAFE.GammatoneProc('-a', 'gammatone3', 'preProc', 44100);
pause(p);

ichProc = rosAFE.IhcProc('-a', 'ihc', 'gammatone', 44100);
pause(p);

ildProc = rosAFE.IldProc('-a', 'ild', 'ihc', 44100);
pause(p);

newParam = rosAFE.ModifyParameter('-a','preProc', 'bRemoveDC', '3');

inputLeft = zeros(sampleRate*bufferSize_s, 1);
inputRight = zeros(sampleRate*bufferSize_s, 1);

time = timeVector ( inputLeft, sampleRate );

rp = rosAFE.runningProcessors();
rp.runningProcessors

disp('Go go go');

while(1)
    tic
    inputToMatlab = rosAFE.TDSPorts('input');
    preProc1ToMatlab = rosAFE.TDSPorts('preProc');

    inputLeft = cell2mat(inputToMatlab.TDSPorts.left);
    inputRight = cell2mat(inputToMatlab.TDSPorts.right);

    ppLeft = cell2mat(preProc1ToMatlab.TDSPorts.left);
    ppRight = cell2mat(preProc1ToMatlab.TDSPorts.right);
    
    subplot(2,2,1);
    plot(time, inputLeft);  
    axis([min(time) max(time) -1 1]);

    subplot(2,2,2);
    plot(time, inputRight);
    axis([min(time) max(time) -1 1]);
    
    subplot(2,2,3);
    plot(time, ppLeft);  
    axis([min(time) max(time) -1 1]);

    subplot(2,2,4);
    plot(time, ppRight);
    axis([min(time) max(time) -1 1]);    
    toc
    pause(0.1);

end
pause(3);
rosAFE.RemoveProcessor('-a','ihc');
pause(3);
rosAFE.RemoveProcessor('-a','gammatone');
pause(3);
%rosAFE.RemoveProcessor('-a','input');
rosAFE.RemoveProcessor('-a','preProc');
%rosAFE.RemoveProcessor('-a','gammatone');
%audiowrite('output.wav',[inputLeft inputRight],sampleRate);
pause(3);
rosAFE.RemoveProcessor('-a','preProc1');
pause(3);
rosAFE.Stop('-a');
pause(3);
rosAFE.kill('-a');

