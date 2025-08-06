

%filename = '..\\guitarSamples\\purple.mp3';
%filename = '..\\guitarSamples\\renShort.mp3';
filename = '..\\guitarSamples\\wonderS.mp3';

bufferLength = 1024;

irResolution = bufferLength*15;

wet = 0.9;

audioOut = [];

input = dsp.AudioFileReader(filename, 'SamplesPerFrame', bufferLength);
output = audioDeviceWriter('SampleRate', input.SampleRate);
Fs = input.SampleRate;

%[IR, IRFs] = audioread('..\\reverbIRs\\Lexicon.wav');
%[IR, IRFs] = audioread('..\\reverbIRs\\vocalPlate.wav');
[IR, IRFs] = audioread('..\\reverbIRs\\emt250.wav');


if IRFs ~= input.SampleRate
    IR = resample(IR, input.SampleRate, IRFs);
end

irLength = length(IR);
fftLength = 2^nextpow2(bufferLength + irResolution - 1);

IRfftL = fft(IR(:,1), fftLength);
IRfftR = fft(IR(:,2), fftLength);

overlapBufferL = zeros(fftLength - bufferLength, 1);
overlapBufferR = zeros(fftLength - bufferLength, 1);

window = hamming(bufferLength);

while ~isDone(input)

    buffer = input();
   
    bufferWin = buffer;% .* window;

    bufferfftL = fft(bufferWin(:,1), fftLength);
    bufferfftR = fft(bufferWin(:,2), fftLength);
    
    convResultfftL = bufferfftL .* IRfftL;
    convResultfftR = bufferfftR .* IRfftR;
    
    convResultL = ifft(convResultfftL, fftLength, 'symmetric');
    convResultR = ifft(convResultfftR, fftLength, 'symmetric');
    
    convResultL(1:length(overlapBufferL)) = convResultL(1:length(overlapBufferL)) + overlapBufferL;
    convResultR(1:length(overlapBufferR)) = convResultR(1:length(overlapBufferR)) + overlapBufferR;
    
    reverb = [convResultL(1:bufferLength), convResultR(1:bufferLength)];
    reverb = reverb/8;
    
    output(wet*reverb+(1-wet)*buffer);
    audioOut = [audioOut; reverb;];
    
    overlapBufferL = convResultL(bufferLength+1:end);
    overlapBufferR = convResultR(bufferLength+1:end);
end

release(input);
release(output);

%audiowrite('overlap-purple-Lexicon.wav', audioOut, input.SampleRate);
