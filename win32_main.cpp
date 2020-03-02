
#include <string>
#include <vector>
#include <random>

#include <Windows.h>
#include <mmdeviceapi.h>
#include <Audioclient.h>

#include "Debug.h"

#include "GLAD/glad.h"
#include "GLFW/glfw3.h"
#include "EngineMath.h"
#include "Shaders.h"


struct Color {
	float values[3];
	float* operator[](int index) { return ((&values)[index]); }
};


#include "SpriteBatch.h"
#include "SpriteString.h"

Global IAudioClient* wasapi_AudioClient;
Global IAudioRenderClient* wasapi_RenderClient;

struct win32_soundOutput {
	uint16_t runningSampleIndex;
	uint16_t samplesPerSecond;
	uint16_t bytesPerSample;
	uint16_t bufferSize;
	uint16_t latencySampleCount;
};

struct game_AudioBuffer {
	int sampleRate;
	int sampleCount;
	int channels;
	int bitDepth;
	int bufferSize;
	uint16_t* samples;

};


void window_framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}


//TODO: Put these in an input structure.
//	Maybe make use of the repeat action

struct Keypress {
	bool IsDown;
	bool wasDown;
	int repeatCount;
};

Global Keypress W = {0};
Global Keypress S = {0};
Global Keypress A = {0};
Global Keypress D = {0};

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_W) {
		if (action == GLFW_PRESS) {
			W.IsDown = true;
		}
		if (action == GLFW_RELEASE) {
			W.IsDown = false;
		}
	} 

	if (key == GLFW_KEY_S) {
		if (action == GLFW_PRESS) {
			S.IsDown = true;
		}
		if (action == GLFW_RELEASE) {
			S.IsDown = false;
		}
	}

	if (key == GLFW_KEY_A) {
		if (action == GLFW_PRESS) {
			A.IsDown = true;
		}
		if (action == GLFW_RELEASE) {
			A.IsDown = false;
		}
	} 

	if (key == GLFW_KEY_D) {
		if (action == GLFW_PRESS) {
			D.IsDown = true;
		}
		if (action == GLFW_RELEASE) {
			D.IsDown = false;
		}
	}
		
}

struct Camera {
	Vector2 position;
	float zoom;
};


Local void win32_initWasapi(int32_t samplesPerSecond, int32_t bufferSizeInSamples) {
	if (FAILED(CoInitializeEx(0, COINIT_SPEED_OVER_MEMORY))) {
		
	}

	IMMDeviceEnumerator* Enumerator;
	if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, IID_PPV_ARGS(&Enumerator))))
	{
		
	}

	IMMDevice* Device;
	if (FAILED(Enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &Device)))
	{
		
	}

	if (FAILED(Device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (LPVOID*)&wasapi_AudioClient)))
	{
		
	}

	WAVEFORMATEXTENSIBLE WaveFormat;

	WaveFormat.Format.cbSize = sizeof(WaveFormat);
	WaveFormat.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
	WaveFormat.Format.wBitsPerSample = sizeof(uint16_t) * 8; 
	WaveFormat.Format.nChannels = 2;
	WaveFormat.Format.nSamplesPerSec = (DWORD)samplesPerSecond;
	WaveFormat.Format.nBlockAlign = (WORD)(WaveFormat.Format.nChannels * WaveFormat.Format.wBitsPerSample / 8);
	WaveFormat.Format.nAvgBytesPerSec = WaveFormat.Format.nSamplesPerSec * WaveFormat.Format.nBlockAlign;
	WaveFormat.Samples.wValidBitsPerSample = sizeof(uint16_t) * 8;
	WaveFormat.dwChannelMask = KSAUDIO_SPEAKER_STEREO;
	WaveFormat.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
	// buffer size in 100 nanoseconds
	REFERENCE_TIME BufferDuration = 10000000ULL * bufferSizeInSamples / samplesPerSecond; 
	if (FAILED(wasapi_AudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_NOPERSIST, BufferDuration, 0, &WaveFormat.Format, nullptr)))
	{
		debug_printMsg("Wasapi: Failed audio client init.");
	}

	if (FAILED(wasapi_AudioClient->GetService(IID_PPV_ARGS(&wasapi_RenderClient))))
	{
	}

	UINT32 SoundFrameCount;
	if (FAILED(wasapi_AudioClient->GetBufferSize(&SoundFrameCount)))
	{
		
	}

}

void audioTest_sinWave(int seconds, int sampleRate, float hz) {

	BYTE* soundBufferData;

	float value = 0.0f;
	const float volume = 300.0f;

	int samplesToWrite = seconds * sampleRate;


	float wavePeriod = 2.0f*PI*hz;

	if (SUCCEEDED(wasapi_RenderClient->GetBuffer((UINT32)samplesToWrite, &soundBufferData)))
	{
		uint16_t sourceSample = 0;
		uint16_t* destSample = (uint16_t*)soundBufferData;


		for (int SampleIndex = 0;
			SampleIndex < samplesToWrite;
			++SampleIndex)
		{
			*destSample++ = (uint16_t)(value*volume);
			*destSample++ = (uint16_t)(value*volume);

			value = sinf(wavePeriod*((float)SampleIndex / (float)sampleRate));


		}


		wasapi_RenderClient->ReleaseBuffer((UINT32)samplesToWrite, 0);
	}
}

Local void
win32_writeToAudioBuffer(uint16_t* sourceBuffer, int samplesToWrite)
{
	BYTE* soundBufferData;
	if (SUCCEEDED(wasapi_RenderClient->GetBuffer((UINT32)samplesToWrite, &soundBufferData)))
	{
		uint16_t* sourceSample = sourceBuffer;
		uint16_t* destSample = (uint16_t*)soundBufferData;
		for (int SampleIndex = 0;
			SampleIndex < samplesToWrite;
			++SampleIndex)
		{
			*destSample++ = *sourceSample++; //Left channel
			*destSample++ = *sourceSample++; //Right channel
		}

		wasapi_RenderClient->ReleaseBuffer((UINT32)samplesToWrite, 0);
	}
}



int main() {
	
	/* TODO:
		GL Buffer and shader abstraction
		
		|Other
		-Updating the frame buffer
	*/

	float windowWidth = 1280.0f;
	float windowHeight = 720.0f;

	if (!glfwInit()) {
		debug_printMsg("Failed to initilize window (GLFW3.2)");
	} 

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_SAMPLES, 1);

	GLFWwindow* window = glfwCreateWindow((int)windowWidth,(int)windowHeight, "Cloud Haven", NULL, NULL);

	if (!window) {
		debug_printMsg("Could not create a window");
	}

	/* Init audio 
	const uint32_t AudioLatency = 1;
	const uint32_t GameUpdateHz = 60;

	int audioSampleRate = 48000;
	int samplesPerFrame = AudioLatency*(audioSampleRate / GameUpdateHz);
	int channelSize = sizeof(uint16_t);
	int channels = 2;
	int bytesPerSample = channels * channelSize;
	int byteBufferSize = bytesPerSample * samplesPerFrame;

	win32_initWasapi(audioSampleRate, byteBufferSize);
	wasapi_AudioClient->Start();

	uint16_t* audioBuffer = (uint16_t*)VirtualAlloc(NULL, byteBufferSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

	if (!audioBuffer) {
		debug_printMsg("Failed to allocate audioBuffer.");
	}
*/

	/* Init opengl */
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, window_framebufferSizeCallback);
	glfwSetKeyCallback(window, key_callback);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		debug_printMsg("Failed to load OpenGL functions");
	}

	const GLubyte* glVersion = glGetString(GL_VERSION);
	debug_printMsg((char*)glVersion);





	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glViewport(0, 0, 1024, 768);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	double start_t = 0.0;
	double end_t = 0.0;

	// TODO: Implement Kahan sum?
	double runningTime = 0.0;

	Vector2 inputVector = { 0 };


	// y(x) = e^(-dx) + x/d - 1/(d^2)
	// y(x) = max velocity  : if moving on a unit axis
	// where x is acceleration
	// d = drag coefficient
	float accelerationScalar = .1f;

	float G = 9.78f;
	
	float accelerationScalarY = -G * 10.0f;
	
	bool canJump = true;

	Vector2 drag = { 0 };
	Vector2 acceleration = { 0 };
	Vector2 velocity = { 0 };
	Vector2 position = { 0 };
	

	double mouseX, mouseY;

	Texture spriteSheet("data/SkyHaven.png");
	Texture playerImg("data/player.png");
	Texture bitmapFont("data/EXEPixelPerfect.tatlas");

	uint32_t shader_Sprite = buildAndLinkShaders(vert_Sprite, frag_Sprite);
	uint32_t shader_StringRender = buildAndLinkShaders(vert_RenderString, frag_RenderString);


	SpriteStringBatch stringDrawer(&shader_StringRender, &bitmapFont, 32.0f, 32.0f);

	SpriteString* fps = stringDrawer.CreateSpriteString(Vector2::Origin(), "12345", 32.0f*.5f);
	fps->position = {-(float)windowWidth*.5f + 32.0f, windowHeight*.5f - 32.0f};
	
	SpriteBatch backgrounds(&shader_Sprite, &spriteSheet);
	SpriteBatch entities(&shader_Sprite, &playerImg);

	Sprite* skyHaven = backgrounds.CreateSprite(Vector2::Origin(), 14.0f, 6.0f);
	Sprite* player = entities.CreateSprite(Vector2::Origin(), 1.0f, 2.0f);



	int view = glGetUniformLocation(shader_Sprite, "view");
	int resolution = glGetUniformLocation(shader_Sprite, "resolution");

	Camera camera;
	camera.position = Vector2::Origin();
	camera.zoom = 1.0f;


	float dt = 0.0f;
	glfwSetTime(0.0);

	float volume = 300.0f;
	float sinWavePeriod = 2.0*PI;
	float hz = 200.0f;
	int sampleCounter = 0;


	float phaseAccumulator = 0.0F;
	float dPhi; 
	

	while (!glfwWindowShouldClose(window)) {

		float currentTime = (float)(runningTime);
		start_t = glfwGetTime();


		//TODO: Wrap up input state

		W.wasDown = W.IsDown;
		S.wasDown = S.IsDown;
		A.wasDown = A.IsDown;
		D.wasDown = A.IsDown;
		glfwPollEvents();

		if (W.wasDown && W.IsDown) {
			W.repeatCount++;
		}
		else {
			W.repeatCount = 0;
		}

		if (S.wasDown && S.IsDown) {
			S.repeatCount++;
		}
		else {
			S.repeatCount = 0;
		}
	

		if (A.wasDown && A.IsDown) {
			A.repeatCount++;
		}
		else {
			A.repeatCount = 0;
		}

		if (D.wasDown && D.IsDown) {
			D.repeatCount++;
		}
		else {
			D.repeatCount = 0;
		}

		inputVector = { 0.0f, 0.0f };


		if (W.IsDown) {
			inputVector += Vector2{ 0.0f, 1.0f };
		}
	
		if (S.IsDown) {
			inputVector += Vector2{ 0.0f, -1.0f };
		}

		if (A.IsDown) {
			inputVector += Vector2{ -1.0f, 0.0f };
		}
	
		if (D.IsDown) {
			inputVector += Vector2{ 1.0f, 0.0f };
		}

		
		if (A.IsDown) {
			inputVector += Vector2{ -1.0f, 0.0f };
		}
		if (D.IsDown) {
			inputVector += Vector2{ 1.0f, 0.0f };
		}



/*		dPhi = 2.0f*PI*hz / (float)(audioSampleRate);
		float audioValue;


		uint16_t* destSample = audioBuffer; //uint16_t*)writeLocation;
		for (int sampleIndex = 0; sampleIndex < samplesPerFrame; sampleIndex++) {
			phaseAccumulator += dPhi;
			audioValue = volume * sinf(phaseAccumulator);
			*destSample++ = (uint16_t)audioValue;
			*destSample++ = (uint16_t)audioValue;
		}
		
*/
	//	win32_writeToAudioBuffer(audioBuffer, samplesPerFrame);

		
		//TODO: Move to a kinematics component
		position = .5f*acceleration*dt*dt + velocity * dt + position;
		velocity = acceleration * dt + velocity;
		acceleration = inputVector * accelerationScalar - drag;
		//acceleration += Vector2{ 0.0f, -G * 10.0f };
		drag = 4.0f * velocity;
		velocity = clamp(velocity);
	
		player->Translate(velocity);

		camera.position = player->Position();
	
	
		fps->text = std::to_string(dt);

		
		glClear(GL_COLOR_BUFFER_BIT);

		
		
		//TODO: Does this neeeed to be checked every frame?
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
	
		glUseProgram(shader_Sprite);
		glUniform3f(view, camera.position.x, camera.position.y, camera.zoom);
		glUniform2f(resolution, (float)width, (float)height);


		backgrounds.Draw();
		entities.Draw();


		/* UI DRAWING LAST */
		glUseProgram(shader_StringRender);
		stringDrawer.Draw(windowWidth, windowHeight);
	

		glfwSwapBuffers(window);	


		end_t = glfwGetTime();
		runningTime = end_t;
		
		dt = (float)(end_t - start_t);

	}



	glfwTerminate();

	return 0;
}