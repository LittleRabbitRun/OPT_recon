#include <utility>

#include <boost/python.hpp>
#include <Python.h>
#include <Numeric/arrayobject.h>
#include <ct.h>


PyObject *get_data(const ImageFile &ifile)
{
 int sizes[2] = { ifile.ny(), ifile.nx() };
  char *ptr0, *ptr1;

  PyArrayObject *array = 
    (PyArrayObject *) PyArray_FromDims(2, sizes, PyArray_FLOAT);

  ptr0 = array->data;

  kfloat32** const if_array = ifile.getArray();

  for(int iy=0; iy < ifile.ny(); iy++) {
    ptr1 = ptr0;
    for (int ix=0; ix < ifile.nx(); ix++) {
      *(float *)ptr1 = if_array[ix][iy];
	  //	  printf ("At x = %d, y = %d, the value is %f\n", ix, iy, if_array[ix][iy]);
      ptr1 += array->strides[1];
    }
    ptr0 += array->strides[0];
    
  }

  return (PyObject *) array;
}

void set_data(ImageFile &ifile, PyObject *obj)
{

  int sizes[2] = { ifile.ny(), ifile.nx() };
  char *ptr0, *ptr1;

  PyArrayObject  *array = 
    (PyArrayObject *) PyArray_ContiguousFromObject(obj, PyArray_FLOAT, 2, 2);
  if(array == NULL) {
    PyErr_SetString(PyExc_TypeError, "incorrect array type");
    throw boost::python::error_already_set();
  }

  if(array->dimensions[0] != sizes[0] || array->dimensions[1] != sizes[1]) {
    PyErr_SetString(PyExc_TypeError, 
                    "Array dimension do not match projections");
    throw boost::python::error_already_set();
  }

  ptr0 = array->data;
  for (int iy = 0; iy < ifile.ny(); iy++) {
    ptr1 = ptr0;
	kfloat32** const if_array = ifile.getArray();
	//    DetectorArray& detarray = pj.getDetectorArray (iy);
	//    DetectorValue* detval = detarray.detValues();
    for (int ix = 0; ix < ifile.nx(); ix++) {
      if_array[ix][iy] = *(float *) ptr1;
      ptr1 += array->strides[1];
    }
    ptr0 += array->strides[0];
  }
}

PyObject *get_minmax(const ImageFile &ifile)
{
  double min, max = 0;
  int Dims[1] = {2};
  char *ptr0;

  ifile.getMinMax(min, max);
  PyArrayObject *array = 
	(PyArrayObject *) PyArray_FromDims(1,Dims,PyArray_FLOAT);

  ptr0 = array->data;
  *(float *)ptr0 = min;
  ptr0+= array->strides[0];
  *(float *)ptr0 = max;

  return (PyObject *) array;

}
  

using namespace boost::python;



BOOST_PYTHON_MODULE(libimagefile)
{
  /*
  class_<Array2dFileLabel>("Array2dFileLabel", init<>())
    //    .def("getLabelType", &Array2dFileLabel::getLabelType)
  ;
  */
  class_<Array2dFile,boost::noncopyable>("Array2dFile", init<>())
    .def("fileRead", (bool (Array2dFile::*)(const std::string&)) &Array2dFile::fileRead)
	.def("fileWrite",(bool (Array2dFile::*)(const std::string&)) &Array2dFile::fileWrite)
    .def("nx", &Array2dFile::nx)
    .def("ny", &Array2dFile::ny)
  ;

  class_<ImageFile, boost::noncopyable, bases<Array2dFile> >("ImageFile", init<>())
    .def(init<int, int>())
        // .def("fileRead", (bool (Array2dFile::*)(const std::string&)) &Array2dFile::fileRead)
    //    .def("nx", &Array2dFile::nx)
    //    .def("ny", &Array2dFile::ny)
    .def("get_data",get_data)
	.def("set_data",set_data)
	.def("getMinMax", get_minmax);
  

  ;


  import_array();

}
