#include <cassert>
#include <iostream>
#include <string>

#include <silo.h>

/* This whole module kind of functions like an "object", maintaining a silo
 * state (database pointer), controling how that pointer gets (un)set, and
 * providing methods to write different kinds of data to whatever's at the end
 * of it. Right now, it functions as a singleton class because I haven't thought
 * of a reason why you'd want to write to multiple silo files simultaneously. If
 * that changes, we're either doing something really cool or really crazy. */
DBfile *silo_database = nullptr;

extern "C" {
  void open_silo_(const int &);
  void write_silo_point_mesh_(double [], double [], double[], const int &, const char[]);
  void write_silo_point_data_(double [], const int &, const char [], const char []);
  void write_silo_quad_mesh_(double [], double [], double [], const int &, const int &, const int &, const char []);
  void write_silo_quad_data_(double [], const int &, const int &, const int &, const char [], const char []);
  void close_silo_();
}

void open_silo_(const int &index)
{
  char fname[128]; // I really hate mixing C & C++, but Silo forced me to...
  sprintf(fname, "data/spin%05d.silo", index);
  silo_database = DBCreate(
      fname, DB_CLOBBER, DB_LOCAL, "s3 spin components", DB_HDF5);

  if(!silo_database)
    std::cerr << "Could not open silo file!" << std::endl;
}

//=============================================================

void write_silo_point_mesh_(double x[], double y[], double z[], const int &npts, const char mesh_id[])
{
  assert(silo_database);

  int ndims = 3;
  double *coords[] = {x, y, z};
  DBPutPointmesh(
      silo_database, mesh_id, ndims, coords, npts, DB_DOUBLE, NULL);
}

void write_silo_point_data_(double data[], const int &npts, const char value_label[], const char mesh_id[])
{
  assert(silo_database);

  DBPutPointvar1(
      silo_database, value_label, mesh_id, data, npts, DB_DOUBLE, NULL);
}

//=============================================================

void write_silo_quad_mesh_(
    double xs[], double ys[], double zs[],
    const int &nx, const int &ny, const int &nz,
    const char mesh_id[])
{
  assert(silo_database);

  int ndims = 3, dims[] = {nx, ny, nz};
  double *coords[] = {xs, ys, zs};

  DBPutQuadmesh(
      silo_database, mesh_id, NULL, coords, dims, ndims,
      DB_DOUBLE, DB_COLLINEAR, NULL);
}

void write_silo_quad_data_(
    double data[],
    const int &nx, const int &ny, const int &nz,
    const char value_label[], const char mesh_id[])
{
  assert(silo_database);

  int ndims = 3, dims[] = {nx, ny, nz};
  DBPutQuadvar1(silo_database, value_label, mesh_id, data, dims, ndims,
      NULL, 0, DB_DOUBLE, DB_NODECENT, NULL);
}

//=============================================================

void close_silo_()
{
  assert(silo_database);
  DBClose(silo_database);
  silo_database = nullptr;
}
