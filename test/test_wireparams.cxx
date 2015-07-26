#include "WireCellNav/WireParams.h"
#include "WireCellUtil/Testing.h"

#include "TApplication.h"
#include "TCanvas.h"
#include "TView.h"
#include "TPolyLine3D.h"
#include "TPolyMarker3D.h"

using namespace WireCell;
using namespace std;

int main(int argc, char** argv)
{
    WireParams wp;

    const Ray& bbox = wp.bounds();

    cout << "Bounds: " << bbox << endl;

    cerr << "pU=" << wp.pitchU() << endl;
    cerr << "pV=" << wp.pitchV() << endl;
    cerr << "pW=" << wp.pitchW() << endl;

    TApplication* theApp = 0;
    if (argc > 1) {
	theApp = new TApplication ("test_iwireprovider",0,0);
    }

    TCanvas c;

    TView* view = TView::CreateView(1);
    view->SetRange(0, -10, -10,   5, 10, 10);

    view->ShowAxis();

    const Ray pitch_rays[3] = { wp.pitchU(), wp.pitchV(), wp.pitchW() };
    int colors[3] = {2, 4, 1};

    const Vector xaxis(1,0,0);

    for (int ind=0; ind<3; ++ind) {
	const Ray& pitch_ray = pitch_rays[ind];
	cout << ind << ": " << pitch_ray << endl;

	const Vector pitch_dir = ray_vector(pitch_ray);
	const Vector wire_dir = pitch_dir.cross(xaxis).norm();
	const Vector wire_point = pitch_ray.second + wire_dir;

	TPolyLine3D* pl = new TPolyLine3D(3);
	pl->SetPoint(0, pitch_ray.first.x(), pitch_ray.first.y(), pitch_ray.first.z());
	pl->SetPoint(1, pitch_ray.second.x(), pitch_ray.second.y(), pitch_ray.second.z());
	pl->SetPoint(2, wire_point.x(), wire_point.y(), wire_point.z());
	pl->SetLineColor(colors[ind]);
	pl->Draw();
    }

    if (theApp) {
	theApp->Run();
    }
    else {			// batch
	c.Print("test_wireparams.pdf");
    }


    return 0;

}
