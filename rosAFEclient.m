addpath(genpath('~/openrobots/lib/matlab'));
addpath(genpath('~/matlab_ws'));

client = genomix.client;
bass = client.load('bass');
rosAFE = client.load('rosAFE');
rosAFE.connect_port('Audio', 'bass/Audio');

sampleRate = 44100;
nFramesPerChunk = 2205;
nChunksOnPort = 20;
bufferSize_s = 10;

p=1;
acquire = bass.Acquire('-a', 'hw:2,0', sampleRate, nFramesPerChunk, nChunksOnPort);
pause(p);

get = rosAFE.InputProc('-a', 12000, bufferSize_s, 'input');
pause(p);

preProc = rosAFE.PreProc('-a', 'preProc1', 'input', 44100);
pause(p);

inputLeft = zeros(sampleRate*bufferSize_s, 1);
inputRight = zeros(sampleRate*bufferSize_s, 1);

time = timeVector ( inputLeft, sampleRate );

disp('Go go go');
 
while(1)
    inputToMatlab = rosAFE.inputProcessorOutput();

    inputLeft = cell2mat(inputToMatlab.inputProcessorOutput.left);
    inputRight = cell2mat(inputToMatlab.inputProcessorOutput.right);

    subplot(2,1,1);
    plot(time, inputLeft);  
    axis([min(time) max(time) -1 1]);

    subplot(2,1,2);
    plot(time, inputRight);
    axis([min(time) max(time) -1 1]);

    disp(inputToMatlab.inputProcessorOutput.lastFrameIndex);
    
    pause(0.1);

end

%audiowrite('output.wav',[inputLeft inputRight],sampleRate);

