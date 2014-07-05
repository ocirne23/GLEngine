package oc.gle;

import android.app.Activity;
import android.content.Context;
import android.graphics.Canvas;
import android.graphics.PixelFormat;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.RelativeLayout;

public class SDLActivity2 extends Activity {
	private static final String TAG = "SDL";

	private static SDLActivity2 instance;

	private boolean isPaused = false, isSurfaceReady = false, hasFocus = true;
	private boolean exitCalledFromJava = false;
	private SDLSurface2 surface;
	private ViewGroup layout;
	private Thread sdlThread;

	static {
		System.loadLibrary("SDL2");
		System.loadLibrary("main");
	}

	public static SDLActivity2 getInstance() {
		return instance;
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		Log.v("SDL", "onCreate():" + instance);
		super.onCreate(savedInstanceState);
		
		assert (SDLActivity2.instance == null);
		SDLActivity2.instance = this;

		surface = new SDLSurface2(this);
		layout = new RelativeLayout(this);
		layout.addView(surface);
		setContentView(layout);
	}

	public void onSurfaceChanged(int width, int height, int format) {
		SDLActivity2.onNativeResize(width, height, format);
		Log.v("SDL", "Window size:" + width + "x" + height);

		isSurfaceReady = true;
		SDLActivity2.onNativeSurfaceChanged();

		if (sdlThread == null) {
			sdlThread = new Thread(new Runnable() {
				@Override
				public void run() {
					SDLActivity2.nativeInit();
					Log.v("SDL", "SDL thread terminated");
				}

			}, "SDLThread");

			sdlThread.start();

			new Thread(new Runnable() {
				@Override
				public void run() {
					try {
						sdlThread.join();
					} catch (Exception e) {
					} finally {
						if (!exitCalledFromJava) {
							SDLActivity2.handleNativeExit();
						}
					}
				}
			}).start();
		}
	}

	@Override
	protected void onPause() {
		Log.v("SDL", "onPause()");
		super.onPause();
		pause();
	}

	@Override
	protected void onResume() {
		Log.v("SDL", "onResume()");
		super.onResume();
		resume();
	}

	@Override
	public void onWindowFocusChanged(boolean hasFocus) {
		super.onWindowFocusChanged(hasFocus);
		Log.v("SDL", "onWindowFocusChanged(): " + hasFocus);

		this.hasFocus = hasFocus;
		if (hasFocus) {
			resume();
		}
	}

	@Override
	public void onLowMemory() {
		Log.v("SDL", "onLowMemory()");
		super.onLowMemory();
		SDLActivity2.nativeLowMemory();
	}

	@Override
	protected void onDestroy() {
		Log.v("SDL", "onDestroy()");

		exitCalledFromJava = true;
		SDLActivity2.nativeQuit();

		if (sdlThread != null) {
			try {
				sdlThread.join();
			} catch (Exception e) {
				Log.v("SDL", "Problem stopping thread: " + e);
			}
			sdlThread = null;
		}

		super.onDestroy();
	}

	@Override
	public boolean dispatchKeyEvent(KeyEvent event) {
		int keyCode = event.getKeyCode();

		if (keyCode == KeyEvent.KEYCODE_VOLUME_DOWN || keyCode == KeyEvent.KEYCODE_VOLUME_UP || keyCode == KeyEvent.KEYCODE_CAMERA || keyCode == KeyEvent.KEYCODE_ZOOM_IN
				|| keyCode == KeyEvent.KEYCODE_ZOOM_OUT) {
			return false;
		}
		return super.dispatchKeyEvent(event);
	}

	public void pause() {
		if (!isPaused && isSurfaceReady) {
			isPaused = true;
			isSurfaceReady = false;
			SDLActivity2.nativePause();
		}
	}

	public void resume() {
		Log.w("SDL", "resumin");
		if (isPaused && isSurfaceReady && hasFocus) {
			isPaused = false;
			SDLActivity2.nativeResume();
		}
	}

	private void exit() {
		sdlThread = null;
		finish();
	}

	public void onSurfaceDestroyed() {
		surface = null;
	}
	
    protected boolean onUnhandledMessage(int command, Object param) {
        return false;
    }

	public static native void nativeInit();
	public static native void nativeLowMemory();
	public static native void nativeQuit();
	public static native void nativePause();
	public static native void nativeResume();
	public static native void onNativeResize(int x, int y, int format);
	public static native int onNativePadDown(int device_id, int keycode);
	public static native int onNativePadUp(int device_id, int keycode);
	public static native void onNativeJoy(int device_id, int axis, float value);
	public static native void onNativeHat(int device_id, int hat_id, int x, int y);
	public static native void onNativeKeyDown(int keycode);
	public static native void onNativeKeyUp(int keycode);
	public static native void onNativeKeyboardFocusLost();
	public static native void onNativeTouch(int touchDevId, int pointerFingerId, int action, float x, float y, float p);
	public static native void onNativeAccel(float x, float y, float z);
	public static native void onNativeSurfaceChanged();
	public static native void onNativeSurfaceDestroyed();
	public static native void nativeFlipBuffers();
	public static native int nativeAddJoystick(int device_id, String name, int is_accelerometer, int nbuttons, int naxes, int nhats, int nballs);
	public static native int nativeRemoveJoystick(int device_id);
	
	public static void handleNativeExit() {
		instance.exit();
	}
	
	public static void flipBuffers() {
		SDLActivity2.nativeFlipBuffers();
	}

	public static Context getContext() {
		return instance;
	}
	
	public static boolean setActivityTitle(String title) {
        return true;
    }

    public static boolean sendMessage(int command, int param) {
        return true;
    }

	public static Surface getNativeSurface() {
		return SDLActivity2.getInstance().surface.getNativeSurface();
	}
	
    public static int audioInit(int sampleRate, boolean is16Bit, boolean isStereo, int desiredFrames) {
        return 0;
    }
    
    public static void audioWriteShortBuffer(short[] buffer) {

    }
    
    public static void audioWriteByteBuffer(byte[] buffer) {

    }

    public static void audioQuit() {

    }

    public static int[] inputGetInputDeviceIds(int sources) {
        return new int[0];
    }
            
    public static boolean handleJoystickMotionEvent(MotionEvent event) {
        return true;
    }
    
    public static void pollInputDevices() {

    }
}

class SDLSurface2 extends SurfaceView implements SurfaceHolder.Callback, View.OnKeyListener, View.OnTouchListener, SensorEventListener {

	private SDLActivity2 sdlActivity;
	private float width, height;

	// Startup
	public SDLSurface2(SDLActivity2 sdlActivity) {
		super(sdlActivity);
		
		this.sdlActivity = sdlActivity;
		getHolder().addCallback(this);
		setFocusable(true);
		setFocusableInTouchMode(true);
		requestFocus();
		setOnKeyListener(this);
		setOnTouchListener(this);

		// Some arbitrary defaults to avoid a potential division by zero
		width = 1.0f;
		height = 1.0f;
	}

	public Surface getNativeSurface() {
		return getHolder().getSurface();
	}

	@SuppressWarnings("deprecation")
	@Override
	public void surfaceCreated(SurfaceHolder holder) {
		Log.v("SDL", "surfaceCreated()");
		holder.setType(SurfaceHolder.SURFACE_TYPE_GPU);
	}

	@Override
	public void surfaceDestroyed(SurfaceHolder holder) {
		Log.v("SDL", "surfaceDestroyed()");

		SDLActivity2 sdlActivity = SDLActivity2.getInstance();
		if (sdlActivity != null) {
			sdlActivity.pause();
			sdlActivity.onSurfaceDestroyed();
		}

		SDLActivity2.onNativeSurfaceDestroyed();
	}

	@SuppressWarnings("deprecation")
	@Override
	public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
		Log.v("SDL", "surfaceChanged()");

		int sdlFormat = 0x16161804; //888
		switch (format) {
		case PixelFormat.A_8:
			Log.v("SDL", "pixel format A_8");
			break;
		case PixelFormat.LA_88:
			Log.v("SDL", "pixel format LA_88");
			break;
		case PixelFormat.L_8:
			Log.v("SDL", "pixel format L_8");
			break;
		case PixelFormat.RGBA_4444:
			Log.v("SDL", "pixel format RGBA_4444");
			sdlFormat = 0x15421002; // SDL_PIXELFORMAT_RGBA4444
			break;
		case PixelFormat.RGBA_5551:
			Log.v("SDL", "pixel format RGBA_5551");
			sdlFormat = 0x15441002; // SDL_PIXELFORMAT_RGBA5551
			break;
		case PixelFormat.RGBA_8888:
			Log.v("SDL", "pixel format RGBA_8888");
			sdlFormat = 0x16462004; // SDL_PIXELFORMAT_RGBA8888
			break;
		case PixelFormat.RGBX_8888:
			Log.v("SDL", "pixel format RGBX_8888");
			sdlFormat = 0x16261804; // SDL_PIXELFORMAT_RGBX8888
			break;
		case PixelFormat.RGB_332:
			Log.v("SDL", "pixel format RGB_332");
			sdlFormat = 0x14110801; // SDL_PIXELFORMAT_RGB332
			break;
		case PixelFormat.RGB_565:
			Log.v("SDL", "pixel format RGB_565");
			sdlFormat = 0x15151002; // SDL_PIXELFORMAT_RGB565
			break;
		case PixelFormat.RGB_888:
			Log.v("SDL", "pixel format RGB_888");
			// Not sure this is right, maybe SDL_PIXELFORMAT_RGB24 instead?
			sdlFormat = 0x16161804; // SDL_PIXELFORMAT_RGB888
			break;
		default:
			Log.v("SDL", "pixel format unknown " + format);
			break;
		}

		this.width = width;
		this.height = height;
		sdlActivity.onSurfaceChanged(width, height, sdlFormat);
	}

	@Override
	public boolean onTouch(View v, MotionEvent event) {
		final int touchDevId = event.getDeviceId();
		final int pointerCount = event.getPointerCount();
		int action = event.getActionMasked();
		int pointerFingerId;
		int i = -1;
		float x, y, p;

		switch (action) {
		case MotionEvent.ACTION_MOVE:
			for (i = 0; i < pointerCount; i++) {
				pointerFingerId = event.getPointerId(i);
				x = event.getX(i) / width;
				y = event.getY(i) / height;
				p = event.getPressure(i);
				SDLActivity2.onNativeTouch(touchDevId, pointerFingerId, action, x, y, p);
			}
			break;

		case MotionEvent.ACTION_UP:
		case MotionEvent.ACTION_DOWN:
			// Primary pointer up/down, the index is always zero
			i = 0;
		case MotionEvent.ACTION_POINTER_UP:
		case MotionEvent.ACTION_POINTER_DOWN:
			// Non primary pointer up/down
			if (i == -1) {
				i = event.getActionIndex();
			}

			pointerFingerId = event.getPointerId(i);
			x = event.getX(i) / width;
			y = event.getY(i) / height;
			p = event.getPressure(i);
			SDLActivity2.onNativeTouch(touchDevId, pointerFingerId, action, x, y, p);
			break;
		}

		return true;
	}

	@Override
	public void onDraw(Canvas canvas) {
	}

	@Override
	public void onAccuracyChanged(Sensor sensor, int accuracy) {
	}

	@Override
	public void onSensorChanged(SensorEvent event) {
	}

	@Override
	public boolean onKey(View arg0, int arg1, KeyEvent arg2) {
		return false;
	}
}
