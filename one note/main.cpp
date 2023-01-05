#define _USE_MATH_DEFINES
#include <iostream>
#include <cmath>
#include <fstream>
using namespace std;

const int sampleRate = 44100;
const int bitDepth = 16;


class SineOscillator {

	private:
		float frequency;
		float amplitude;
		float offset;
		float angle = 0.0f;

	public:
		SineOscillator(float frequency, float amplitude) {
			this->frequency = frequency;
			this->amplitude = amplitude;
			offset = 2 * M_PI * frequency / sampleRate;
		}

		float process() {
			// A*sin(2*PI*f/sr)
			auto sample = amplitude * sin(angle);
			angle += offset;
			return sample;
		}
};

void writeToFile(ofstream &file,int value,int size) {
	file.write(reinterpret_cast<char*>(&value), size);
}

int main() {

	int duration = 2;
	ofstream audioFile;
	audioFile.open("waveform.wav",ios::binary);
	SineOscillator so = SineOscillator(240, 0.5);

	//header chunk
	audioFile << "RIFF";
	audioFile << "____";
	audioFile << "WAVE";

	//format chunk
	audioFile << "fmt ";
	writeToFile(audioFile, 16, 4); // size
	writeToFile(audioFile, 1, 2); //compression code
	writeToFile(audioFile, 1, 2); //number of channels
	writeToFile(audioFile, sampleRate, 4); //sample rate
	writeToFile(audioFile, sampleRate * bitDepth / 8, 4); //byte rate
	writeToFile(audioFile, bitDepth / 8, 2); //block allign
	writeToFile(audioFile, bitDepth, 2); //bit depth

	//data chunk
	audioFile << "data";
	audioFile << "____";

	int preAudioPos = audioFile.tellp();
	auto maxAmplitude = pow(2, bitDepth - 1) - 1;
	for (int i = 0; i < sampleRate * duration; i++) {
		auto  sample = so.process();
		int intSample = static_cast<int>(sample * maxAmplitude);
		writeToFile(audioFile, intSample, 2);
	}
	int postAudioPos = audioFile.tellp();


	audioFile.seekp(preAudioPos-4);
	writeToFile(audioFile, postAudioPos - preAudioPos, 4);

	audioFile.seekp(4,ios::beg);
	writeToFile(audioFile, postAudioPos - 8, 4);


	audioFile.close();
	return 0;
}