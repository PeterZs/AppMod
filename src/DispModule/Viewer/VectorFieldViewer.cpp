#include "VectorFieldViewer.h"
#include "VectorFieldCanvas.h"
#include <qevent.h>
#include "FeatureGuided.h"
#include "FeatureLine.h"
//#include <cv.h>
//#include <eigen\dense>

VectorFieldViewer::VectorFieldViewer(QWidget* widget)
  : BasicViewer(widget)
{
  is_drawAllLines = false;
  is_drawLine = false;
  delete_target_mode = false;

  interaction_mode = VectorField::DRAW_CRSP_LINE;
  selected_v_id = -1;
}

VectorFieldViewer::~VectorFieldViewer()
{

}

void VectorFieldViewer::draw()
{
  if(delete_target_mode == true)
  {
    //startScreenCoordinatesSystem(true);
    for (size_t i = 0; i < dispObjects.size(); ++i)
    {
      VectorFieldCanvas* canvas = dynamic_cast<VectorFieldCanvas*>(dispObjects[i]);
      if (canvas)
      {
        int w = this->width();
        int h = this->height();
        canvas->deleteTargetCurves(line, w, h, proj_mat_out);
       /* CURVES target_curves;
        canvas->getTargetCurves(target_curves);
        std::vector<std::vector<std::pair<int, int>>> target_curves_screen;
        target_curves_screen.resize(target_curves.size());
        for(size_t i = 0; i < target_curves.size(); i ++)
        {
          target_curves_screen[i].resize(target_curves[i].size());
          for(size_t j = 0; j < target_curves[i].size(); j ++)
          {
            qreal src[3], res[3];
            src[0] = target_curves[i][j].x;
            src[1] = target_curves[i][j].y;
            src[2] = 0;
            camera()->getProjectedCoordinatesOf(src, res);
            target_curves_screen[i][j].first = int(res[0]);
            target_curves_screen[i][j].second = int(res[1]);
          }
        }*/
      }
    }
    line.clear();
    //stopScreenCoordinatesSystem();
    delete_target_mode = false;
  }
  
  BasicViewer::draw();

  glClear(GL_DEPTH_BUFFER_BIT);
  drawLine();
  drawSelectedLine();

  if (is_drawAllLines)
  {
    glClear(GL_DEPTH_BUFFER_BIT);
    drawAllLines();

  }
  
  
}

void VectorFieldViewer::init()
{
  BasicViewer::init();

  qglClearColor(QColor(Qt::white));
  setBackgroundColor(QColor(Qt::white));
  setForegroundColor(QColor(Qt::white));

  //set camera
  camera()->setType(qglviewer::Camera::Type::ORTHOGRAPHIC);
  camera()->setPosition(qglviewer::Vec(0.5, 0.5, 1.0));

  //clearMouseBindings();
  setWheelBinding(Qt::NoModifier, CAMERA, MOVE_FORWARD);
}

//void VectorFieldViewer::postSelection(const QPoint& point)
//{
//   //std::cout << "The selected point is :" << point.x() <<" , " << point.y() << std::endl;
//   qreal src[3],res[3];
//   src[0] = point.x();
//   src[1] = point.y();
//   src[2] = 0.5;
//   camera()->getUnprojectedCoordinatesOf(src,res);
//
//   std::cout << "The UnprojectedCoordinates of the selected point is:" << res[0] << "," << res[1] << "," << res[2] << std::endl;
//   /*GLdouble mvpm[16],mvm[16];
//   camera()->getModelViewProjectionMatrix(mvpm);
//   camera()->getProjectionMatrix(mvm);
//   std::cout << "The ModelViewMatrix is : " << std::endl 
//     << mvm[0] << "," << mvm[1] << "," << mvm[2] << "," << mvm[3] << std::endl
//     << mvm[4] << "," << mvm[5] << "," << mvm[6] << "," << mvm[7] << std::endl
//     << mvm[8] << "," << mvm[9] << "," << mvm[10] << "," << mvm[11] << std::endl
//     << mvm[12] << "," << mvm[13] << "," << mvm[14] << "," << mvm[15] << std::endl;
//   Eigen::MatrixXd mvpm_mat(4,4);
//   mvpm_mat << mvpm[0],mvpm[1],mvpm[2],mvpm[3],
//     mvpm[4],mvpm[5],mvpm[6],mvpm[7],
//     mvpm[8],mvpm[9],mvpm[10],mvpm[11],
//     mvpm[12],mvpm[13],mvpm[14],mvpm[15];
//   Eigen::MatrixXd topLeftPoint(4,1),topLeftPoint_projected(4,1);
//   topLeftPoint << 0,1,0,1;
//   topLeftPoint_projected = mvpm_mat * topLeftPoint;
//   double topLeft_x = topLeftPoint_projected(0,0);
//   double topLeft_y = topLeftPoint_projected(1,0);
//   std::cout << "The projected top left point is :" << topLeft_x << "," << topLeft_y << "," << topLeftPoint_projected(2,0) << "," << topLeftPoint_projected(3,0) << std::endl;
//   std::cout << "The camera position is :" << camera()->position().x << "," << camera()->position().y << "," << camera()->position().z << std::endl;*/
//   
//}

void VectorFieldViewer::updateSourceField(int update_type)
{
  for (size_t i = 0; i < dispObjects.size(); ++i)
  {
    VectorFieldCanvas* canvas = dynamic_cast<VectorFieldCanvas*>(dispObjects[i]);
    if (canvas)
    {
      canvas->updateSourceField(update_type);
    }
  }
}

void VectorFieldViewer::updateScalarFieldTexture()
{
  makeCurrent();

  for (size_t i = 0; i < dispObjects.size(); ++i)
  {
    VectorFieldCanvas* canvas = dynamic_cast<VectorFieldCanvas*>(dispObjects[i]);
    if (canvas)
    {
      canvas->setScalarField();
    }
  }

  updateGL();
  doneCurrent();
}


void VectorFieldViewer::mousePressEvent(QMouseEvent *e)
{
  if ((e->button() == Qt::LeftButton) && (e->modifiers() == Qt::NoButton))
  {
    // get coordinates
    qreal src[3],res[3];
    src[0] = e->x();
    src[1] = e->y();
    src[2] = 0.5;
    camera()->getUnprojectedCoordinatesOf(src,res);

    if (interaction_mode == VectorField::DRAW_CRSP_LINE || interaction_mode == VectorField::DELETE_TARGET_CURVES || interaction_mode == VectorField::ADD_TARGET_CURVES)
    {
      is_drawLine = true;
      double2 point;
      point.x = res[0];
      point.y = res[1];
      line.push_back(point);

      //std::cout << "The UnprojectedCoordinates of the selected point is :" << point.x << "," << point.y << std::endl;
    }
    else if (interaction_mode == VectorField::SELECT_POINT || interaction_mode == VectorField::CORRECT_CRSP)
    {
      // get the v_id
      user_start[0] = res[0];
      user_start[1] = res[1];
      selected_v_id = 0;
    }
  }
  else
  {
    QGLViewer::mousePressEvent(e);
  }
}

void VectorFieldViewer::mouseMoveEvent(QMouseEvent *e)
{

  int handled = 0;
  if (interaction_mode == VectorField::DRAW_CRSP_LINE || interaction_mode == VectorField::DELETE_TARGET_CURVES || interaction_mode == VectorField::ADD_TARGET_CURVES)
  {
    if(is_drawLine)
    {
      if(!line.empty())
      {
        double2 previousPoint = line[line.size() - 1];
        double2 currentPoint;
        qreal src[3],res[3];
        src[0] = e->x();
        src[1] = e->y();
        src[2] = 0.5;
        camera()->getUnprojectedCoordinatesOf(src,res);
        currentPoint.x = res[0];
        currentPoint.y = res[1];
        //std::cout << "The UnprojectedCoordinates of the selected point is :" << currentPoint.x << "," << currentPoint.y << std::endl;
        if(sqrt(pow(currentPoint.x - previousPoint.x,2) + pow(currentPoint.y - previousPoint.y,2)) > 0.001)
          line.push_back(currentPoint);
        updateGLOutside();

      }
      handled = 1;
    }
  }
  else if (interaction_mode == VectorField::SELECT_POINT || interaction_mode == VectorField::CORRECT_CRSP)
  {
    //if (selected_v_id != -1)
    //{
    //  // call projection optimization
    //}
  }

  if (handled == 0)
  {
    QGLViewer::mouseMoveEvent(e);
  }
}

void VectorFieldViewer::mouseReleaseEvent(QMouseEvent *e)
{
  if ((e->button() == Qt::LeftButton) && (e->modifiers() == Qt::NoButton))
  {
    if (interaction_mode == VectorField::DRAW_CRSP_LINE)
    {
      //std::cout << "The points of the drawline are :" << std::endl;
      //for(int i = 0;i < line.size();i ++)
      //  std::cout << line[i].x << "," << line[i].y << std::endl;
      is_drawLine = false;

      // pass the new line into feature guided model
      for (size_t i = 0; i < dispObjects.size(); ++i)
      {
        VectorFieldCanvas* canvas = dynamic_cast<VectorFieldCanvas*>(dispObjects[i]);
        if (canvas)
        {
          canvas->addConstrainedLines(line, selected_line);          
        }
      }
      updateGLOutside();
      line.clear();
      selected_line.clear();
    }
    else if (interaction_mode == VectorField::DELETE_TARGET_CURVES)
    {
      is_drawLine = false;
      delete_target_mode = true;
     
      GLfloat modelview[16];
      GLfloat projection[16];
      GLint viewport[4];
      camera()->getModelViewMatrix(modelview);
      camera()->getProjectionMatrix(projection);
      camera()->getViewport(viewport);
      proj_mat_out.setZero();
      /*proj_mat_out(0, 0) = m_viewport(2) / 2;
      proj_mat_out(0, 3) = m_viewport(0) + m_viewport(2) / 2;
      proj_mat_out(1, 1) = -m_viewport(3) / 2;
      proj_mat_out(1, 3) = -m_viewport(3) / 2;*/
      proj_mat_out(0, 0) = viewport[2] / 2;
      proj_mat_out(0, 3) = viewport[0] + viewport[2] / 2;
      proj_mat_out(1, 1) = -viewport[3] / 2;
      proj_mat_out(1, 3) = -viewport[3] / 2;
      proj_mat_out(3, 3) = 1.0;

      Matrix4f m_modelview;
      Matrix4f m_projection;

      m_modelview = Eigen::Map<Eigen::Matrix4f>(modelview, 4, 4);
      m_projection = Eigen::Map<Eigen::Matrix4f>(projection, 4, 4);

      proj_mat_out = proj_mat_out * m_projection * m_modelview;
      

      updateGLOutside();
    }
    else if(interaction_mode == VectorField::ADD_TARGET_CURVES)
    {
      is_drawLine = false;
      for (size_t i = 0; i < dispObjects.size(); ++i)
      {
        VectorFieldCanvas* canvas = dynamic_cast<VectorFieldCanvas*>(dispObjects[i]);
        if (canvas)
        {
          canvas->addTargetCurves(line);
        }
      }
      line.clear();
      updateGLOutside();
    }
    else if (interaction_mode == VectorField::SELECT_POINT || interaction_mode == VectorField::CORRECT_CRSP)
    {
      if (selected_v_id == 0)
      {
        // get coordinates
        qreal src[3],res[3];
        src[0] = e->x();
        src[1] = e->y();
        src[2] = 0.5;
        camera()->getUnprojectedCoordinatesOf(src,res);

        user_end[0] = res[0];
        user_end[1] = res[1];

        // pass the new line into feature guided model
        for (size_t i = 0; i < dispObjects.size(); ++i)
        {
          VectorFieldCanvas* canvas = dynamic_cast<VectorFieldCanvas*>(dispObjects[i]);
          if (canvas)
          {
            canvas->setConstrainedPair(user_start, user_end);
          }
        }

        selected_v_id = -1;
        if (interaction_mode == VectorField::SELECT_POINT)
        {
          emit(triggeredInteractiveCrsp());
        }
        else
        {
          this->updateGLOutside();
        }
      }
    }
  }
  else
  {
    QGLViewer::mouseReleaseEvent(e);
  }
}


void VectorFieldViewer::drawLine()
{
  if(line.size() > 1)
  {
    glLineWidth(2);
    glBegin(GL_LINE_STRIP);
    for(int i = 0;i  < line.size();i ++)
    {
      glColor3f(0,1,0);
      glVertex3f(line[i].x,line[i].y,0);
    }
    glEnd();
  } 
}

void VectorFieldViewer::drawSelectedLine()
{
  if(selected_line.size() > 1)
  {
    glLineWidth(5);
    glBegin(GL_LINE_STRIP);
    for(int i = 0;i  < selected_line.size();i ++)
    {
      glColor3f(0.5,0,0.5);
      glVertex3f(selected_line[i].x,selected_line[i].y,0);
    }
    glEnd();
  } 
}

void VectorFieldViewer::deleteLastLine()
{
  for (size_t i = 0; i < dispObjects.size(); ++i)
  {
    VectorFieldCanvas* canvas = dynamic_cast<VectorFieldCanvas*>(dispObjects[i]);
    if (canvas)
    {
      canvas->deleteLastLine();
    }
  }
}

void VectorFieldViewer::isDrawAllLines(bool allLines)
{
  is_drawAllLines = allLines;
}

void VectorFieldViewer::drawAllLines()
{
  for (size_t i = 0; i < dispObjects.size(); ++i)
  {
    VectorFieldCanvas* canvas = dynamic_cast<VectorFieldCanvas*>(dispObjects[i]);
    if (canvas)
    {
      std::shared_ptr<FeatureLine> feature_lines = canvas->getFeatureLine();
      for(size_t j = 0; j < feature_lines->lines.size(); j++)
      {
        glBegin(GL_LINE_STRIP);
        for(size_t k = 0; k < (feature_lines->lines)[j].size(); k ++)
        {
          glColor3f(0, 1, 0);
          glVertex3f((feature_lines->lines)[j][k].x, (feature_lines->lines)[j][k].y, 0);
        }
        glEnd();
      }
    }
  }
}

void VectorFieldViewer::setDispPara(std::vector<bool>& states)
{
  for (size_t i = 0; i < dispObjects.size(); ++i)
  {
    VectorFieldCanvas* canvas = dynamic_cast<VectorFieldCanvas*>(dispObjects[i]);
    if (canvas)
    {
      canvas->setVisualizationParas(states);
    }
  }
}

bool VectorFieldViewer::inDrawLineMode()
{
  if (interaction_mode == VectorField::DRAW_CRSP_LINE)
  {
   return true;
  }

  return false;
}

bool VectorFieldViewer::inSelectPointMode()
{
  if (interaction_mode == VectorField::SELECT_POINT)
  {
    return true;
  }

  return false;
}