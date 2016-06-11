#ifndef __ABOUT_WINDOW_H__
#define __ABOUT_WINDOW_H__

#include "ui_about_window.h"
#include "gfx_renderer.h"
#include <QTimer>

class AboutWindow : public QMainWindow
{
  Q_OBJECT

public:

  AboutWindow(boost::asio::io_service& io);

private:

  void closeEvent(QCloseEvent *event);

  QFont getFont();

  void handleFrameRendered(GfxRenderer::Frame::Ref frame);

  void keyPressEvent(QKeyEvent *event);

private slots:

  void handleFrameFlip();

private:

  class Shader : public GfxRenderer::Shader
  {
  public:
    virtual GfxMath::vec3 shade(const GfxMath::vec2& fragCoord, const GfxMath::vec2& fragRes, const float time);
  };

  Ui::AboutWindow m_ui;

  GfxRenderer::Ref m_gfxRenderer;
  std::vector<QPixmap> m_frames;
  QTimer* m_player;
  size_t m_frameIndex;

};

#endif // __ABOUT_WINDOW_H__
