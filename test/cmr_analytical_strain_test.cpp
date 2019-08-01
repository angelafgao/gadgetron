#include "gtest/gtest.h"
#include "complext.h"
#include "hoNDArray_reductions.h"
#include "hoNDArray_elemwise.h"
#include "hoNFFT.h"
#include "cmr_analytical_strain.h"
#include "ImageIOAnalyze.h"
#include "GadgetronTimer.h"

using namespace Gadgetron;
using testing::Types;

template<typename Real>
class cmr_analytical_strain_test : public ::testing::Test
{
protected:
	virtual void SetUp()
	{
		GDEBUG_STREAM("=============================================================================================");
		char* ut_dir = ::getenv("GT_UNITTEST_DIRECTORY");
		if (ut_dir != NULL)
		{
			gt_ut_folder_ = std::string(ut_dir);
		}
		else
			gt_ut_folder_.clear();

		GDEBUG_STREAM("=============================================================================================");

		if (!gt_ut_folder_.empty())
		{
			GDEBUG_STREAM("Unit Test for Gadgetron hoNFFT");
			gt_ut_data_folder_ = gt_ut_folder_;
			gt_ut_res_folder_ = gt_ut_folder_ + "/result/";
			GDEBUG_STREAM("gt_ut_data_folder_ is " << gt_ut_data_folder_);
			GDEBUG_STREAM("gt_ut_res_folder_ is " << gt_ut_res_folder_);
		}

		this->timer_.set_timing_in_destruction(false);
	}

	std::string gt_ut_folder_;
	std::string gt_ut_data_folder_;
	std::string gt_ut_res_folder_;

	ImageIOAnalyze gt_io_;
	GadgetronTimer timer_;
};

typedef Types<float> realImplementations;

TYPED_TEST_CASE(cmr_analytical_strain_test, realImplementations);

TYPED_TEST(cmr_analytical_strain_test, QuadarticStrain)
{
	typedef float T;

	if (this->gt_ut_folder_.empty())
	{
		GDEBUG_STREAM("Gadgetron unit test directory is not set ... ");
		return;
	}

	hoNDArray< T > dx;
	this->gt_io_.import_array(dx, this->gt_ut_data_folder_ + "/SynthesisStrain/grtr_dx");
	dx.print(std::cout);

	hoNDArray< T > dy;
	this->gt_io_.import_array(dy, this->gt_ut_data_folder_ + "/SynthesisStrain/grtr_dy");
	dy.print(std::cout);

	hoNDArray< T > mask;
	this->gt_io_.import_array(mask, this->gt_ut_data_folder_ + "/SynthesisStrain/mask");
	T norm_mask = Gadgetron::nrm2(mask);

	hoNDArray<T> radial, circ;
	Gadgetron::compute_analytical_strain(dx, dy, mask, radial, circ);

	this->gt_io_.export_array(radial, this->gt_ut_res_folder_ + "/AnalyticalQuad/radial");
	this->gt_io_.export_array(circ, this->gt_ut_res_folder_ + "/AnalyticalQuad/circ");

	// compare agains ground truth
	hoNDArray<T> ref;
	hoNDArray<T> diff;
	T norm_ref;

	this->gt_io_.import_array(ref, this->gt_ut_data_folder_ + "/analytical_results/analytical_quad_rad_strain");
	std::cout << Gadgetron::nrm2(radial) << std::endl;
	std::cout << Gadgetron::nrm2(ref) << std::endl;
	Gadgetron::subtract(ref, radial, diff);
	T v = Gadgetron::nrm2(diff);
	norm_ref = Gadgetron::nrm2(ref);
	EXPECT_LE(v / norm_ref, 0.002);

	this->gt_io_.import_array(ref, this->gt_ut_data_folder_ + "/analytical_results/analytical_quad_circ_strain");
	std::cout << Gadgetron::nrm2(circ) << std::endl;
	std::cout << Gadgetron::nrm2(ref) << std::endl;
	Gadgetron::subtract(ref, circ, diff);
	T q = Gadgetron::nrm2(diff);
	norm_ref = Gadgetron::nrm2(ref);
	EXPECT_LE(q / norm_ref, 0.002);
}

TYPED_TEST(cmr_analytical_strain_test, ConstStrain)
{
	typedef float T;

	if (this->gt_ut_folder_.empty())
	{
		GDEBUG_STREAM("Gadgetron unit test directory is not set ... ");
		return;
	}

	hoNDArray< T > dx;
	this->gt_io_.import_array(dx, this->gt_ut_data_folder_ + "/SynthesisStrain/cons_grtr_dx");
	dx.print(std::cout);

	hoNDArray< T > dy;
	this->gt_io_.import_array(dy, this->gt_ut_data_folder_ + "/SynthesisStrain/cons_grtr_dy");
	dy.print(std::cout);

	hoNDArray< T > mask;
	this->gt_io_.import_array(mask, this->gt_ut_data_folder_ + "/SynthesisStrain/cons_mask");
	T norm_mask = Gadgetron::nrm2(mask);

	hoNDArray<T> radial, circ;
	Gadgetron::compute_analytical_strain(dx, dy, mask, radial, circ);

	this->gt_io_.export_array(radial, this->gt_ut_res_folder_ + "/AnalyticalCons/radial");
	this->gt_io_.export_array(circ, this->gt_ut_res_folder_ + "/AnalyticalCons/circ");

	// compare agains ground truth
	hoNDArray<T> ref;
	hoNDArray<T> diff;
	T norm_ref;

	this->gt_io_.import_array(ref, this->gt_ut_data_folder_ + "/analytical_results/analytical_cons_rad_strain");
	std::cout << Gadgetron::nrm2(radial) << std::endl;
	std::cout << Gadgetron::nrm2(ref) << std::endl;
	Gadgetron::subtract(ref, radial, diff);
	T v = Gadgetron::nrm2(diff);
	norm_ref = Gadgetron::nrm2(ref);
	EXPECT_LE(v / norm_ref, 0.002);

	this->gt_io_.import_array(ref, this->gt_ut_data_folder_ + "/analytical_results/analytical_cons_circ_strain");
	std::cout << Gadgetron::nrm2(circ) << std::endl;
	std::cout << Gadgetron::nrm2(ref) << std::endl;
	Gadgetron::subtract(ref, circ, diff);
	T q = Gadgetron::nrm2(diff);
	norm_ref = Gadgetron::nrm2(ref);
	EXPECT_LE(q / norm_ref, 0.002);
}

TYPED_TEST(cmr_analytical_strain_test, RetroCine_slice)
{
	typedef float T;

	if (this->gt_ut_folder_.empty())
	{
		GDEBUG_STREAM("Gadgetron unit test directory is not set ... ");
		return;
	}

	hoNDArray< T > dx;
	this->gt_io_.import_array(dx, this->gt_ut_data_folder_ + "/RetroCine/dx_6_6");
	dx.print(std::cout);

	hoNDArray< T > dy;
	this->gt_io_.import_array(dy, this->gt_ut_data_folder_ + "/RetroCine/dy_6_6");
	dy.print(std::cout);

	hoNDArray< T > mask;
	this->gt_io_.import_array(mask, this->gt_ut_data_folder_ + "/RetroCine/mask_6");
	T norm_mask = Gadgetron::nrm2(mask);

	hoNDArray<T> radial, circ;
	Gadgetron::compute_analytical_strain(dx, dy, mask, radial, circ);

	this->gt_io_.export_array(radial, this->gt_ut_res_folder_ + "/AnalyticalCineSlice/radial");
	this->gt_io_.export_array(circ, this->gt_ut_res_folder_ + "/AnalyticalCineSlice/circ");

	// compare agains ground truth
	hoNDArray<T> ref;
	hoNDArray<T> diff;
	T norm_ref;

	this->gt_io_.import_array(ref, this->gt_ut_data_folder_ + "/analytical_results/analytical_cine_6_6_rad_strain");
	std::cout << Gadgetron::nrm2(radial) << std::endl;
	std::cout << Gadgetron::nrm2(ref) << std::endl;
	Gadgetron::subtract(ref, radial, diff);
	T v = Gadgetron::nrm2(diff);
	norm_ref = Gadgetron::nrm2(ref);
	EXPECT_LE(v / norm_ref, 0.002);

	this->gt_io_.import_array(ref, this->gt_ut_data_folder_ + "/analytical_results/analytical_cine_6_6_circ_strain");
	std::cout << Gadgetron::nrm2(circ) << std::endl;
	std::cout << Gadgetron::nrm2(ref) << std::endl;
	Gadgetron::subtract(ref, circ, diff);
	T q = Gadgetron::nrm2(diff);
	norm_ref = Gadgetron::nrm2(ref);
	EXPECT_LE(q / norm_ref, 0.002);
}



TYPED_TEST(cmr_analytical_strain_test, RetroCine)
{
	typedef float T;

	if (this->gt_ut_folder_.empty())
	{
		GDEBUG_STREAM("Gadgetron unit test directory is not set ... ");
		return;
	}

	hoNDArray< T > dx;
	this->gt_io_.import_array(dx, this->gt_ut_data_folder_ + "/RetroCine/dx_6");
	dx.print(std::cout);

	hoNDArray< T > dy;
	this->gt_io_.import_array(dy, this->gt_ut_data_folder_ + "/RetroCine/dy_6");
	dy.print(std::cout);

	hoNDArray< T > mask;
	this->gt_io_.import_array(mask, this->gt_ut_data_folder_ + "/RetroCine/mask_6");
	T norm_mask = Gadgetron::nrm2(mask);

	hoNDArray<T> radial, circ;
	Gadgetron::compute_analytical_strain(dx, dy, mask, radial, circ);

	this->gt_io_.export_array(radial, this->gt_ut_res_folder_ + "/AnalyticalCine/radial");
	this->gt_io_.export_array(circ, this->gt_ut_res_folder_ + "/AnalyticalCine/circ");

	// compare agains ground truth
	hoNDArray<T> ref;
	hoNDArray<T> diff;
	T norm_ref;

	this->gt_io_.import_array(ref, this->gt_ut_data_folder_ + "/analytical_results/analytical_cine_6_rad_strain");
	std::cout << Gadgetron::nrm2(radial) << std::endl;
	std::cout << Gadgetron::nrm2(ref) << std::endl;
	Gadgetron::subtract(ref, radial, diff);
	T v = Gadgetron::nrm2(diff);
	norm_ref = Gadgetron::nrm2(ref);
	EXPECT_LE(v / norm_ref, 0.002);

	this->gt_io_.import_array(ref, this->gt_ut_data_folder_ + "/analytical_results/analytical_cine_6_circ_strain");
	std::cout << Gadgetron::nrm2(circ) << std::endl;
	std::cout << Gadgetron::nrm2(ref) << std::endl;
	Gadgetron::subtract(ref, circ, diff);
	T q = Gadgetron::nrm2(diff);
	norm_ref = Gadgetron::nrm2(ref);
	EXPECT_LE(q / norm_ref, 0.002);
}


// with respect to slice 10
TYPED_TEST(cmr_analytical_strain_test, RTCine)
{
	typedef float T;

	if (this->gt_ut_folder_.empty())
	{
		GDEBUG_STREAM("Gadgetron unit test directory is not set ... ");
		return;
	}

	hoNDArray< T > dx;
	this->gt_io_.import_array(dx, this->gt_ut_data_folder_ + "/RTCine/229201050_229201055_362_20190718_dx");
	dx.print(std::cout);

	hoNDArray< T > dy;
	this->gt_io_.import_array(dy, this->gt_ut_data_folder_ + "/RTCine/229201050_229201055_362_20190718_dy");
	dy.print(std::cout);

	hoNDArray< T > mask;
	this->gt_io_.import_array(mask, this->gt_ut_data_folder_ + "/RTCine/229201050_229201055_362_20190718_mask");
	T norm_mask = Gadgetron::nrm2(mask);

	hoNDArray<T> radial, circ;
	Gadgetron::compute_analytical_strain(dx, dy, mask, radial, circ);

	this->gt_io_.export_array(radial, this->gt_ut_res_folder_ + "/AnalyticalRtCine/radial");
	this->gt_io_.export_array(circ, this->gt_ut_res_folder_ + "/AnalyticalRtCine/circ");

	// compare agains ground truth
	hoNDArray<T> ref;
	hoNDArray<T> diff;
	T norm_ref;

	this->gt_io_.import_array(ref, this->gt_ut_data_folder_ + "/analytical_results/analytical_rt_rad_strain");
	std::cout << Gadgetron::nrm2(radial) << std::endl;
	std::cout << Gadgetron::nrm2(ref) << std::endl;
	Gadgetron::subtract(ref, radial, diff);
	T v = Gadgetron::nrm2(diff);
	norm_ref = Gadgetron::nrm2(ref);
	EXPECT_LE(v / norm_ref, 0.002);

	this->gt_io_.import_array(ref, this->gt_ut_data_folder_ + "/analytical_results/analytical_rt_circ_strain");
	std::cout << Gadgetron::nrm2(circ) << std::endl;
	std::cout << Gadgetron::nrm2(ref) << std::endl;
	Gadgetron::subtract(ref, circ, diff);
	T q = Gadgetron::nrm2(diff);
	norm_ref = Gadgetron::nrm2(ref);
	EXPECT_LE(q / norm_ref, 0.002);
}