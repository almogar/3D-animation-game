#include "tutorial/sandBox/sandBox.h"
#include "igl/edge_flaps.h"
#include "igl/collapse_edge.h"
#include "Eigen/dense"
#include <functional>


#include <igl/directed_edge_orientations.h>
#include <igl/forward_kinematics.h>
#include <igl/dqs.h>
#include <iostream>
#include <set>
#include <igl/opengl/glfw/renderer.h>
#include <igl/opengl/glfw/renderer.cpp>
using namespace Eigen;
using namespace igl;
using namespace std;
using namespace opengl;

SandBox::SandBox()
{


}


void SandBox::Init(const std::string& config)
{
    //start comment Project
    std::string item_name;
    std::ifstream nameFileout;
    doubleVariable = 0;

    right = false;// so when press space, it start move to the right
    left = false;
    up = false;
    down = false;
    in = false;
    out = false;
    isTargetMoveing = false;
    isChangeCamera2 = false;

    joints_num = 16;
    skelton.resize(joints_num + 1);
    scale = 1;
    vT.resize(joints_num + 1);
    vQ.resize(joints_num + 1);
    //origin_skelton.resize(joints_num + 1);
    parents.resize(joints_num + 1);
    //origin_vT.resize(17);
    //origin_vQ.resize(17);


    nameFileout.open(config);
    if (!nameFileout.is_open())
    {
        std::cout << "Can't open file " << config << std::endl;
    }
    else
    {

        while (nameFileout >> item_name)
        {
            std::cout << "openning " << item_name << std::endl;
            load_mesh_from_file(item_name);

            Eigen::RowVector3d center(0, 0, -0.8);
            parents.push_back(-1);
            data().add_points(Eigen::RowVector3d(0, 0, 0), Eigen::RowVector3d(0, 0, 1));
            data().show_overlay_depth = false;
            //data().show_faces = true;
            data().point_size = 10;
            data().line_width = 2;
            data().set_visible(false, 1);

            Initialize_Tree(selected_data_index);

            data().SetCenterOfRotation(center.transpose());
            if (selected_data_index == 0) {
                
                V = data().V;
            }
                
        }
        nameFileout.close();
    }
    //data_list[data_list.size() -1].MyTranslate(Eigen::Vector3d(0, -0.8, -1), true);
    //data_list[0].MyRotate(Eigen::Vector3d(1, 0, 0), 90);
    //data_list[0].MyScale(Eigen::Vector3d(0.5, 0.5, 0.5));
    //data_list[0].MyTranslate(Eigen::Vector3d(0, 1, -0.7), true);

    /*for (int i = 0; i < 5; i++) {
        data_list[i].MyTranslate(Eigen::Vector3d(0, 1.6, 0), true);
        data_list[i].SetCenterOfRotation(Eigen::Vector3d(0, -0.8, 0));

        if (i > 0) {
            Movable* pare = &data_list[i - 1];
            data_list[i].setParent(pare);
        }
    }*/

    double z = -0.8 * scale;
    for (int i = 0; i < skelton.size(); i++)
    {
        skelton.at(i) = Eigen::Vector3d(0, 0, z);
        z = z + 0.1 * scale;
    }


    //MyTranslate(Eigen::Vector3d(0, 0, -1), true);

    //Find points for skeleton
    calc_all_weights();
    data().MyRotate(Eigen::Vector3d(0, 1, 0), 3.14 / 2);
    data_list[0].MyRotate(Eigen::Vector3d(0, 1, 0), 3.14 / 2);

    Joints.emplace_back();
    Joints.at(0).MyTranslate(skelton.at(0), true);
    //Joints.at(0).SetCenterOfRotation(Eigen::Vector3d(0, 0, -0.8));
    parents[0] = -1;
    //the 16 other joint that have parents
    for (int i = 0; i < joints_num; i++)
    {
        parents[i + 1] = i;
        Joints.emplace_back();
        Joints.at(i + 1).MyTranslate(skelton.at(i + 1), true);
        //Joints.at(i).SetCenterOfRotation(Eigen::Vector3d(0, 0, -0.8));
        //std::cout << parents[i + 1] <<"\n";
    }

    
    //target_pose = skelton[joints_num];
    U = V;
    for (int i = 0; i < joints_num + 1; i++)
    {
        vT[i] = skelton[i];
    }
    vT[joints_num] = (Joints[joints_num].MakeTransd() * Eigen::Vector4d(0, 0, 0, 1)).head(3);
    //vT[joints_num] = vT[joints_num] + target_pose;
    for (size_t i = 0; i < joints_num; i++)
    {
        vT[i] = vT[i] + (vT[i + 1] - vT[i]) * 0.1;
    }


    igl::dqs(V, W, vQ, vT, U);
    data_list.at(0).set_vertices(U);

    //update skelton   target_pose
    for (int i = 0; i < skelton.size(); i++) {
        skelton[i] = vT[i];
    }
    for (int i = 0; i < joints_num; i++)
    {
        Joints[i].MyTranslate(skelton[i], true);
    }
    /*for (int i = 0; i < skelton.size(); i++)
    {
        origin_skelton.at(i) = skelton.at(i);
        origin_vT.at(i) = vT.at(i);
        origin_vQ.at(i) = vQ.at(i);
    }*/


    //load_mesh_from_file("C:/Users/Almogi/Desktop/animation_course-master/tutorial/data/cube1.obj");
    //init_target_object(selected_data_index);
    //data().MyScale(Eigen::Vector3d(5, 5, 5));
    //data().MyTranslate(Eigen::Vector3d(0, 0, 0), true);

}

SandBox::~SandBox()
{

}


Eigen::VectorXd SandBox::create_weight_vec(double w1, double w1_ind, double w2, double w2_ind)
{
    Eigen::VectorXd Wi;
    Wi.resize(17);

    for (double i = 0; i < 17; i++)
    {
        if (i == w1_ind)
            Wi[w1_ind] = w1;
        else {
            if (i == w2_ind)
                Wi[w2_ind] = w2;
            else
                Wi[i] = 0;
        }
    }
    return Wi;
}

void  SandBox::calc_all_weights()
{
    int verticeSize = data_list[0].V.rows();
    Eigen::MatrixXd V = data_list[0].V;
    W.resize(verticeSize, 17);
    double z_axe_coord, w_1, w_2, lower_bound, upper_bound;

    for (int i = 0; i < verticeSize; i++) {
        z_axe_coord = V.row(i)[2];
        lower_bound = (floor(z_axe_coord * 10)) / 10;
        upper_bound = (ceil(z_axe_coord * 10)) / 10;
        w_1 = abs(z_axe_coord - upper_bound) * 10;
        w_2 = 1 - w_1;
        W.row(i) = create_weight_vec(w_1, lower_bound * 10 + 8, w_2, upper_bound * 10 + 8);
    }
}

void  SandBox::calc_next_pos()
{
    vT[0] = skelton[0];
    for (int i = 0; i < joints_num; i++) {
        vT[i + 1] = skelton[i + 1];
        vT[i] = vT[i] + ((vT[i + 1] - vT[i]) / 6);
    }
    vT[joints_num] = vT[joints_num] + target_pose;
}

//calc from article "automatic skinning weight retargeting 2017"
void SandBox::add_weights() {
    double distance;
    int numOfV = data_list.at(0).V.rows();
    Eigen::MatrixXd V = data_list.at(0).V;
    W.resize(numOfV, 17);

    for (int i = 0; i < data_list[0].V.rows(); i++) {
        double sum = 0;
        double distance;

        for (int j = 0; j < skelton.size(); j++) {
            distance = abs(skelton.at(j).z() - data_list[0].V.row(i).z());
            if (distance <= 0.1) {
                sum += pow((1 / distance), 4);
            }
        }

        // calc sum of sigma on  k=0 to n(jointd) (1/distance(i,k)^4)
        for (int j = 0; j < skelton.size(); j++) {
            distance = abs(skelton.at(j).z() - data_list[0].V.row(i).z());
            double temp = pow((1 / distance), 4);
            W(i, j) = temp / sum;
        }
        W.row(i).normalized();
    }
}

void SandBox::updateMovement() {
    /*if (left) {
        target_pose = Eigen::Vector3d(0.05, 0, 0);
    }
    else if (right) {
        target_pose = Eigen::Vector3d(-0.05, 0, 0);
    }
    else if (up) {
        target_pose = Eigen::Vector3d(0, 0.05, 0);
    }
    else if (down) {
        target_pose = Eigen::Vector3d(0, -0.05, 0);
    }*/

    if (left)
    {
        Joints[joints_num].MyTranslateInSystem(Joints[joints_num].MakeTransd(), Eigen::Vector3d(0.05, 0, 0), true);
        
    }
    if (right)
    {

        Joints[joints_num].MyTranslateInSystem(Joints[joints_num].MakeTransd(), Eigen::Vector3d(-0.05, 0, 0), true);

    }
    if (up)
    {
        Joints[joints_num].MyTranslateInSystem(Joints[joints_num].MakeTransd(), Eigen::Vector3d(0, 0.05, 0), true);

    }
    if (down)
    {
        Joints[joints_num].MyTranslateInSystem(Joints[joints_num].MakeTransd(), Eigen::Vector3d(0, -0.05, 0), true);
    }
    if (in)
    {

        Joints[joints_num].MyTranslateInSystem(Joints[joints_num].MakeTransd(), Eigen::Vector3d(0, 0, 0.05), true);

    }
    if (out)
    {

        Joints[joints_num].MyTranslateInSystem(Joints[joints_num].MakeTransd(), Eigen::Vector3d(0, 0, -0.05), true);
    }
}


//Project levels  functions
void SandBox::levelk()
{
    if (score >= targetScore * level) {
        isNextLevel = true;
        isActive = false;
       
        //remove targets
        for (int i = 1; i < data_list.size(); i++)
            data_list[i].clear();

        // OV keeping the first vertics we had to the snake
        data_list[0].set_vertices(data_list[0].OV);

        //retrieve original values of the snake, original vertices kept in OV variable
        for (int i = 0; i < skelton.size(); i++)
        {
            //skelton.at(i) = origin_skelton.at(i);
            //vT.at(i) = origin_vT.at(i);
            //vQ.at(i) = origin_vQ.at(i);
        }
    }
    else {
        generate_target(level);
        
    }

}
std::vector<Movable> SandBox::GetJoints() { return Joints; }

void SandBox::Animate()
{
    if (isActive && !isResume && (left || right || up || down || in || out))
    {
        //Move Snake
        //std::cout << "" << "\n";
        updateMovement();

        //find current vT values

        /*vT[0] = skelton[0];
        for (int i = 0; i < joints_num; i++) {
            vT[i + 1] = skelton[i + 1];
            vT[i] = vT[i] + ((vT[i + 1] - vT[i]) / 6);
        }
        vT[joints_num] = vT[joints_num] + target_pose;*/  // lynn
        //std::cout << (Joints[joints_num].MakeTransd() * Eigen::Vector4d(0, 0, 0, 1)).head(3) << "\n";
        
        for (int i = 0; i < joints_num + 1; i++)
        {
            vT[i] = skelton[i];
        }
        vT[joints_num] = (Joints[joints_num].MakeTransd() * Eigen::Vector4d(0, 0, 0, 1)).head(3);
        //vT[joints_num] = vT[joints_num] + target_pose;
        for (size_t i = 0; i < joints_num; i++)
        {
            vT[i] = vT[i] + (vT[i + 1] - vT[i]) * 0.1;
        }
        
        
        igl::dqs(V, W, vQ, vT, U);
        data_list.at(0).set_vertices(U);

        //update skelton   target_pose
        for (int i = 0; i < skelton.size(); i++) {
            skelton[i] = vT[i];
        }
        for (int i = 0; i < joints_num; i++)

        {
            Joints[i].MyTranslate(skelton[i], true);
        }
        //if(data_list.size() > 6)
        //FabrikAlgo(0);
        //IKSimulation();

        

        //if (isTargetMoveing)
            move_targets(level);
        
        if (counter % 3 == 0) {
            data_list[0].tree.init(data_list[0].V, data_list[0].F);
            igl::AABB<Eigen::MatrixXd, 3> tree_first = data_list[0].tree;
            Eigen::AlignedBox<double, 3> box_first = tree_first.m_box;
            checkCollision();
        }
        if (counter % 50 == 0) {

            levelk();


        }

        counter++;
        
        
         // need to prevent check collision to earn more points than target points

    }
}
