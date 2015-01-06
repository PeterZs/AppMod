#ifndef Model_H
#define Model_H


#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <limits>

#include <Eigen\Eigen>
#include <Eigen\Sparse>
#include <cv.h>
#include <highgui.h>

#include "ModelLight.h"
#include "Bound.h"
#include "Viewer.h"
#include "tiny_obj_loader.h"
#include "obj_writer.h"
#include "ModelLight.h"
#include "Ray.h"
#include "Light.h"


class Viewer;

class Model
{
public:
    typedef std::vector<float> VectorF;
    typedef std::vector<unsigned int> Facelist;
    typedef std::vector<float> Colorlist;
    typedef std::vector<float> Normallist;
    typedef std::vector<std::vector<int>> FaceAdjlist;
    typedef std::vector<std::vector<int>> VertexAdjlist;
    typedef Eigen::Vector3i Vector3i;
    typedef Eigen::Vector3f Vector3f;
    typedef std::vector<Vector3i> VectorVec3i;
    typedef std::vector<Vector3f> VectorVec3f;
    

public:
    Model(){};
    ~Model();

    Model(const int id, const std::string path, const std::string name);
    void passData(VectorF &vertices, Facelist &faces, Colorlist &colors);
	void exportOBJ();

    void computeLight();
    void computeVisbs(Eigen::Vector3f point, Eigen::Vector3f normal, std::vector<bool> &visb);
    void computeVisbs(Eigen::Vector3f point, Eigen::Vector3f normal, Eigen::VectorXf &visb);
    void computeVisbs(int face_id, std::vector<bool> &visb);
    void computeVisbs(int face_id, Eigen::VectorXf &visb);
	void computeFaceNormal();

    void passCameraPara(float c_modelview[16], float c_projection[16], int c_viewport[4]);
    void passRenderImgInfo(cv::Mat &zImg, cv::Mat &primitiveID, cv::Mat &rImg);
    void passVerticesVisbleStatus(std::vector<bool> &visble);
    bool getWorldCoord(Eigen::Vector3f rimg_coord, Eigen::Vector3f &w_coord);
    void getPtNormalInFace(Eigen::Vector3f pt, int face_id, Eigen::Vector3f &normal);
    inline void setRenderer(Viewer *viewer){ renderer = viewer; };

    inline ModelLight *getModelLightObj(){ return model_light; };
    inline std::vector<int> *getFaceAdj(int face_id){ return &model_faces_adj[face_id]; };
    inline Bound getBounds(){ return model_bounds; };
    inline std::vector<unsigned int> *getFaceList(){ return &model_faces; };
    inline std::vector<float> *getVertexList(){ return &model_vertices; };
    inline std::vector<float> *getNormalList(){ return &model_normals; };
	inline std::vector<float> *getFaceNormalList(){ return &model_face_normals; };
    inline std::vector<float> *getRhoList(){ return &model_rhos; };
    inline std::vector<std::vector<int>> *getVertexShareFaces(){ return &model_vertices_share_faces; };
    inline std::vector<int> *getVertexShareFaces(int vertex_id){ return &model_vertices_share_faces[vertex_id]; };
    inline std::vector<std::vector<int>> *getVertexAdj(){ return &model_vertex_adj; };
    inline std::vector<Eigen::Vector3i> *getFaceListCompact(){ return &model_faces_compact; };

    inline cv::Mat &getRImg(){ return r_img; };

    inline std::string getDataPath(){ return data_path + std::to_string(ID); };

protected:
    bool loadOBJ(const std::string name, const std::string base_path);
    bool getUnprojectPt(float winx, float winy, float winz, float object_coord[3]);
    bool getProjectPt(float object_coord[3], float &winx, float &winy);
    void computeBaryCentreCoord(float pt[3], float v0[3], float v1[3], float v2[3], float lambd[3]);
    void buildFaceAdj();
    void buildVertexShareFaces();
    void buildVertexAdj();
    void buildFaceListCompact();
    bool shareEdge(int face0, int face1);
    void computeBounds();

protected:
    int ID;
    std::string data_path;
    std::string file_name;

    // model basic attributes
    VectorF model_vertices;
    VectorVec3f model_vertices_compact;
    Facelist model_faces;
    VectorVec3i model_faces_compact;

    // color attributes
    Colorlist model_colors;
    Colorlist model_rhos;
    Colorlist model_brightness;

    // normals
    Normallist model_normals;
    Normallist model_face_normals;

    // auxiliary attributes
    FaceAdjlist model_faces_adj;// face adjacent list
    FaceAdjlist model_vertices_share_faces;// vertex one-ring faces
    VertexAdjlist model_vertex_adj;// vertex adjacent list
    
    Bound model_bounds;

    // Lighting
    ModelLight *model_light;
    std::vector<std::vector<bool>> model_visbs;
	Ray *ray_cast;
    bool shadow_on;

    // information from renderer
    cv::Mat z_img;
    cv::Mat primitive_ID;
    cv::Mat r_img;
    std::vector<bool> v_vis_stat_in_r_img;
    Viewer *renderer;

    // information of camera
    Eigen::Matrix4f m_modelview;
    Eigen::Matrix4f m_projection;
    Eigen::Matrix4f m_inv_modelview_projection;
    Eigen::Vector4i m_viewport;
};

#endif