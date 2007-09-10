#include "pseudoCamera.h"

extern boost::mutex mutexCout;

PseudoCamera::PseudoCamera(int totalImagens,unsigned frequencia,char* diretorio)
{
    this->endImagem = diretorio;
    this->imagemAtual = 1;
    this->totalImagens = 1000;//totalImagens;
    this->atraso = (int)((1.0/(double)frequencia)*1.0e9);
}

PseudoCamera::~PseudoCamera()
{}

void PseudoCamera::Iniciar(unsigned serial)
{}

void PseudoCamera::Finalizar()
{}

void PseudoCamera::SalvarImagem()
{}

double PseudoCamera::CapturarImg(IplImage *imgRetorno)
{
    boost::xtime xt;
    boost::xtime_get(&xt, boost::TIME_UTC);
    xt.nsec += this->atraso;   
    boost::thread::sleep(xt);

    // Limpa a IplImage recebida para o retorno
    cvReleaseImage( &imgRetorno );
  
    ostringstream s1;

    if(this->imagemAtual > this->totalImagens )
    {
       boost::mutex::scoped_lock lock(mutexCout);       
       cout << "A pseudoCamera terminou" << endl;
       return 0;
    } 

//--------------------------------------------------------
s1 << this->endImagem << this->imagemAtual << ".bmp";
cout << "---------------------" <<endl << endl << s1.str() << endl;
//--------------------------------------------------------
    imgRetorno = cvLoadImage( s1.str().c_str(), CV_LOAD_IMAGE_COLOR );

    this->imagemAtual++;

    return 0;
}
