#include <dlfcn.h>
#include <camera/Camera.h>
#include "CameraWrapper.h"

namespace android {

typedef sp<CameraHardwareInterface> (*OpenCamFunc)(int);

static void * g_lilgLibHandle = NULL;
static OpenCamFunc g_lilgOpenCameraHardware = NULL;

static void ensurelilgLibOpened()
{
    if (g_lilgLibHandle == NULL) {
        g_lilgLibHandle = ::dlopen("liblilgcamera.so", RTLD_NOW);
        if (g_lilgLibHandle == NULL) {
            assert(0);
            LOGE("%s: dlopen() error: %s\n", __func__, dlerror());
        } else {
            g_lilgOpenCameraHardware = (OpenCamFunc) ::dlsym(g_lilgLibHandle, "openCameraHardware");
            assert(g_lilgOpenCameraHardware != NULL);
        }
    }
}

extern "C" int HAL_getNumberOfCameras()
{
    return 1;
}

extern "C" void HAL_getCameraInfo(int cameraId, struct CameraInfo* cameraInfo)
{
    cameraInfo->facing = CAMERA_FACING_BACK;
    cameraInfo->orientation = 0;
}

extern "C" sp<CameraHardwareInterface> HAL_openCameraHardware(int cameraId)
{
    LOGV("openCameraHardware: call createInstance");
    ensurelilgLibOpened();
    return CameraWrapper::createInstance(cameraId);
}

wp<CameraHardwareInterface> CameraWrapper::singleton;

sp<CameraHardwareInterface> CameraWrapper::createInstance(int cameraId)
{
    LOGV("%s :", __func__);
    if (singleton != NULL) {
        sp<CameraHardwareInterface> hardware = singleton.promote();
        if (hardware != NULL) {
            return hardware;
        }
    }

    ensurelilgLibOpened();

    sp<CameraHardwareInterface> hardware(new CameraWrapper(cameraId));
    singleton = hardware;
    return hardware;
}

CameraWrapper::CameraWrapper(int cameraId)
{
    mlilgInterface = g_lilgOpenCameraHardware(cameraId);
}

CameraWrapper::~CameraWrapper()
{
}

sp<IMemoryHeap>
CameraWrapper::getPreviewHeap() const
{
    return mlilgInterface->getPreviewHeap();
}

sp<IMemoryHeap>
CameraWrapper::getRawHeap() const
{
    return mlilgInterface->getRawHeap();
}

void
CameraWrapper::setCallbacks(notify_callback notify_cb,
                                  data_callback data_cb,
                                  data_callback_timestamp data_cb_timestamp,
                                  void* user)
{
    mlilgInterface->setCallbacks(notify_cb, data_cb, data_cb_timestamp, user);
}

void
CameraWrapper::enableMsgType(int32_t msgType)
{
    mlilgInterface->enableMsgType(msgType);
}

void
CameraWrapper::disableMsgType(int32_t msgType)
{
    mlilgInterface->disableMsgType(msgType);
}

bool
CameraWrapper::msgTypeEnabled(int32_t msgType)
{
    return mlilgInterface->msgTypeEnabled(msgType);
}

status_t
CameraWrapper::startPreview()
{
    return mlilgInterface->startPreview();
}

bool
CameraWrapper::useOverlay()
{
    return mlilgInterface->useOverlay();
}

status_t
CameraWrapper::setOverlay(const sp<Overlay> &overlay)
{
    return mlilgInterface->setOverlay(overlay);
}

void
CameraWrapper::stopPreview()
{
    mlilgInterface->stopPreview();
}

bool
CameraWrapper::previewEnabled()
{
    return mlilgInterface->previewEnabled();
}

status_t
CameraWrapper::startRecording()
{
    return mlilgInterface->startRecording();
}

void
CameraWrapper::stopRecording()
{
    mlilgInterface->stopRecording();
}

bool
CameraWrapper::recordingEnabled()
{
    return mlilgInterface->recordingEnabled();
}

void
CameraWrapper::releaseRecordingFrame(const sp<IMemory>& mem)
{
    return mlilgInterface->releaseRecordingFrame(mem);
}

status_t
CameraWrapper::autoFocus()
{
    return mlilgInterface->autoFocus();
}

status_t
CameraWrapper::cancelAutoFocus()
{
    return mlilgInterface->cancelAutoFocus();
}

status_t
CameraWrapper::takePicture()
{
    return mlilgInterface->takePicture();
}

status_t
CameraWrapper::cancelPicture()
{
    return mlilgInterface->cancelPicture();
}

status_t
CameraWrapper::setParameters(const CameraParameters& params)
{
    return mlilgInterface->setParameters(params);
}

CameraParameters
CameraWrapper::getParameters() const
{
    CameraParameters ret = mlilgInterface->getParameters();

    ret.set(CameraParameters::KEY_ZOOM_RATIOS, "100,200,300,400");

    return ret;
}

status_t
CameraWrapper::sendCommand(int32_t cmd, int32_t arg1, int32_t arg2)
{
    return mlilgInterface->sendCommand(cmd, arg1, arg2);
}

void
CameraWrapper::release()
{
    mlilgInterface->release();
}

status_t
CameraWrapper::dump(int fd, const Vector<String16>& args) const
{
    return mlilgInterface->dump(fd, args);
}

}; //namespace android

