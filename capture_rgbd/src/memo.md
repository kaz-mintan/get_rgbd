- 2018/12/21

  - to access depth data, seems to have to check `void NuitrackGLSample::onNewDepthFrame(DepthFrame::Ptr frame)` on L146, NuitrackGLSample.cpp.

  - like the depth data, check `void NuitrackGLSample::onNewRGBFrame()` on L188, NuitrackGLSample.cpp.


    - these functions are referred in a func (void init()) to estiamte skeleton to estimate posture. maybe...

