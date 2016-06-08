#ifndef __ABOUT_WINDOW_H__
#define __ABOUT_WINDOW_H__

#include "ui_about_window.h"
#include "gfx_renderer.h"

class AboutWindow : public QMainWindow
{
  Q_OBJECT

public:

  AboutWindow(boost::asio::io_service& io);

private:

  void closeEvent(QCloseEvent *event);

  void displayFrame(GfxRenderer::Frame::Ref frame);

  class Shader : public GfxRenderer::Shader
  {
  public:
    virtual GfxMath::vec3 shade(const GfxMath::vec2& uv, const float time);
  };


  Ui::AboutWindow m_ui;

  GfxRenderer::Ref m_gfxRenderer;

};

#endif // __ABOUT_WINDOW_H__
