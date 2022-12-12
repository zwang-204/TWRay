#ifndef PARAMSET_H
#define PARAMSET_H

#include "pbrt.h"
#include "geometry.h"
#include <map>


namespace pbrt {

template <typename T>
struct ParamSetItem;

// ParamSet Declarations
class ParamSet {
  public:
    // ParamSet Public Methods
    ParamSet() {}
    void AddFloat(const std::string &, std::unique_ptr<float[]> v,
                  int nValues = 1);
    void AddInt(const std::string &, std::unique_ptr<int[]> v, int nValues);
    void AddBool(const std::string &, std::unique_ptr<bool[]> v, int nValues);
    void AddPoint2f(const std::string &, std::unique_ptr<Point2f[]> v,
                    int nValues);
    void AddVector2f(const std::string &, std::unique_ptr<Vector2f[]> v,
                     int nValues);
    void AddPoint3f(const std::string &, std::unique_ptr<Point3f[]> v,
                    int nValues);
    void AddVector3f(const std::string &, std::unique_ptr<Vector3f[]> v,
                     int nValues);
    void AddNormal3f(const std::string &, std::unique_ptr<Normal3f[]> v,
                     int nValues);
    void AddString(const std::string &, std::unique_ptr<std::string[]> v,
                   int nValues);
    void AddTexture(const std::string &, const std::string &);
    void AddRGBSpectrum(const std::string &, std::unique_ptr<float[]> v,
                        int nValues);
    void AddXYZSpectrum(const std::string &, std::unique_ptr<float[]> v,
                        int nValues);
    void AddBlackbodySpectrum(const std::string &, std::unique_ptr<float[]> v,
                              int nValues);
    void AddSampledSpectrum(const std::string &, std::unique_ptr<float[]> v,
                            int nValues);
    bool EraseInt(const std::string &);
    bool EraseBool(const std::string &);
    bool EraseFloat(const std::string &);
    bool ErasePoint2f(const std::string &);
    bool EraseVector2f(const std::string &);
    bool ErasePoint3f(const std::string &);
    bool EraseVector3f(const std::string &);
    bool EraseNormal3f(const std::string &);
    bool EraseSpectrum(const std::string &);
    bool EraseString(const std::string &);
    bool EraseTexture(const std::string &);
    float FindOneFloat(const std::string &, float d) const;
    int FindOneInt(const std::string &, int d) const;
    bool FindOneBool(const std::string &, bool d) const;
    Point2f FindOnePoint2f(const std::string &, const Point2f &d) const;
    Vector2f FindOneVector2f(const std::string &, const Vector2f &d) const;
    Point3f FindOnePoint3f(const std::string &, const Point3f &d) const;
    Vector3f FindOneVector3f(const std::string &, const Vector3f &d) const;
    Normal3f FindOneNormal3f(const std::string &, const Normal3f &d) const;
    Spectrum FindOneSpectrum(const std::string &, const Spectrum &d) const;
    std::string FindOneString(const std::string &, const std::string &d) const;
    std::string FindTexture(const std::string &) const;
    const float *FindFloat(const std::string &, int *n) const;
    const int *FindInt(const std::string &, int *nValues) const;
    const bool *FindBool(const std::string &, int *nValues) const;
    const Point2f *FindPoint2f(const std::string &, int *nValues) const;
    const Vector2f *FindVector2f(const std::string &, int *nValues) const;
    const Point3f *FindPoint3f(const std::string &, int *nValues) const;
    const Vector3f *FindVector3f(const std::string &, int *nValues) const;
    const Normal3f *FindNormal3f(const std::string &, int *nValues) const;
    const Spectrum *FindSpectrum(const std::string &, int *nValues) const;
    const std::string *FindString(const std::string &, int *nValues) const;
    void ReportUnused() const;
    void Clear();
    std::string ToString() const;
    void Print(int indent) const;

  private:
    friend class TextureParams;
    friend bool shapeMaySetMaterialParameters(const ParamSet &ps);

    // ParamSet Private Data
    std::vector<std::shared_ptr<ParamSetItem<bool>>> bools;
    std::vector<std::shared_ptr<ParamSetItem<int>>> ints;
    std::vector<std::shared_ptr<ParamSetItem<float>>> floats;
    std::vector<std::shared_ptr<ParamSetItem<Point2f>>> point2fs;
    std::vector<std::shared_ptr<ParamSetItem<Vector2f>>> vector2fs;
    std::vector<std::shared_ptr<ParamSetItem<Point3f>>> point3fs;
    std::vector<std::shared_ptr<ParamSetItem<Vector3f>>> vector3fs;
    std::vector<std::shared_ptr<ParamSetItem<Normal3f>>> normals;
    std::vector<std::shared_ptr<ParamSetItem<Spectrum>>> spectra;
    std::vector<std::shared_ptr<ParamSetItem<std::string>>> strings;
    std::vector<std::shared_ptr<ParamSetItem<std::string>>> textures;
    static std::map<std::string, Spectrum> cachedSpectra;
};

template <typename T>
struct ParamSetItem {
    // ParamSetItem Public Methods
    ParamSetItem(const std::string &name, std::unique_ptr<T[]> val,
                 int nValues = 1);

    // ParamSetItem Data
    const std::string name;
    const std::unique_ptr<T[]> values;
    const int nValues;
    mutable bool lookedUp = false;
};

// ParamSetItem Methods
template <typename T>
ParamSetItem<T>::ParamSetItem(const std::string &name, std::unique_ptr<T[]> v,
                              int nValues)
    : name(name), values(std::move(v)), nValues(nValues) {}

}

#endif