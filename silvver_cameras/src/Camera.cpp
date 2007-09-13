#include "Camera.h"

Camera::Camera(const CameraConfig& camConfig,double tempoInicial)
{
    fc[0] = camConfig.fc[0];
    fc[1] = camConfig.fc[1];

    cc[0] = camConfig.cc[0];
    cc[1] = camConfig.cc[1];

    kc[0] = camConfig.kc[0]
    kc[1] = camConfig.kc[1]
    kc[2] = camConfig.kc[2]
    kc[3] = camConfig.kc[3];
    kc[4] = camConfig.kc[4];

    alpha_c = camConfig.alpha_c;

    for (int i=0; i<3; i++)
    {
        for (int j=0; j<3; j++)
        {
            this->H[i][j] = camConfig.H[i][j];
        }
    }

    cameraSerial = camConfig.serial;

    switch(camConfig.modelo)
    {
    case CameraConfig::PGR:
        hardCamera    = new PGRCamera(camConfig.frequencia,camConfig.diretorio,tempoInicial);
        break;
    case CameraConfig::PseudoCam:
        hardCamera    = new PseudoCamera(733,camConfig.frequencia,camConfig.diretorio);
        break;
    }

    imgCamera = cvCreateImage( cvSize(camConfig.resolucao[0],camConfig.resolucao[1]), IPL_DEPTH_8U, 3 );

    //timer = new Timer();
    taxaDeQuadros = 0;
    quadros = 0;
}

Camera::~Camera()
{
    cvReleaseImage(&imgCamera);
    hardCamera->Finalizar();
    //delete hardCamera;
    //delete timer;
}

void Camera::Iniciar()
{
    try
    {
    hardCamera->Iniciar(cameraSerial);
    }
    catch(string erro)
    {
        cout << "Erro: " << erro << endl;
        abort();
    }

    //timer->Start();
}

double Camera::CapturarImagem()
{
    double instante;
    try
    {
     instante = hardCamera->CapturarImg(imgCamera);
    }
    catch(string erro)
    {
        cout << erro << endl;
        return 0;
    }

    quadros++;
    /*if (quadros>=30)
    {
        taxaDeQuadros = float( quadros/timer->ElapsedTime() );
        quadros = 0;
        timer->Start();
    }*/

    return instante;
}

void Camera::Localizar(Posicao &posicao)
{

//------------------------------------- Parâmetros Intrínsecos
    double xc[2];
    xc[0] = posicao.x;
    xc[1] = posicao.y;
    double x[2];
    double x_distort[2]
    x_distort[0] = (xc[0] - cc[0])/fc[0]
    x_distort[1] = (xc[1] - cc[1])/fc[1]

    double delta_x[2]
    double k1, k2, k3, k_radial, p1, p2, r2
    k1 = kc[0]
    k2 = kc[1]
    k3 = kc[4]
    p1 = kc[2]
    p2 = kc[3]
    x[0] = x_distort[0]
    x[1] = x_distort[1]
    for (int i = 0; i < 20; i++

        r2 = x[0]*x[0] + x[1]*x[1]
        k_radial = 1.0 + k1 * r2 + k2 * pow(r2,2) + k3 * pow(r2,3)
        delta_x[0] = 2 * p1 * x[0] * x[1] + p2*(r2 + 2 * pow(x[0],2))
        delta_x[1] = p1*(r2 + 2 * pow(x[1],2)) + 2 * p2 * x[0] * x[1]
        x[0] = (x_distort[0] - delta_x[0])/k_radial
        x[1] = (x_distort[1] - delta_x[1])/k_radial

    posicao.x = x[0]*fc[0] + cc[0];
    posicao.y = x[1]*fc[1] + cc[1];

//------------------------------------- Parâmetros Extrínsecos

    double escala,xPixel,yPixel,xMundo,yMundo;

    xPixel = posicao.x;
    yPixel = posicao.y;

    escala = H[2][0]*xPixel + H[2][1]*yPixel + H[2][2];
    xMundo = (H[0][0]*xPixel +
            H[0][1]*yPixel +
            H[0][2]) / escala;
    yMundo = (H[1][0]*xPixel +
            H[1][1]*yPixel +
            H[1][2]) / escala;

    posicao.x = xMundo;
    posicao.y = yMundo;

}
