//  BY Nicholas, nickadamu@gmail.com
// This a custom gesture recognizer

#include "mediapipe/framework/calculator_framework.h"
#include "mediapipe/framework/formats/landmark.pb.h"
#include "mediapipe/framework/formats/rect.pb.h"
#include "mediapipe/util/render_data.pb.h"
#include <cmath>

namespace mediapipe
{
namespace
{
//constexpr char normRectTag[] = "NORM_RECT";
//constexpr char kNormLandmarksTag[] = "NORM_LANDMARKS";
constexpr char kNormLandmarksTag[] = "NORM_LANDMARKS";
constexpr char kRenderDataTag[] = "RENDER_DATA";
//constexpr char kRenderDataTag[] = "RECOGNIZED_HAND_GESTURE";

} // namespace

class GestureRecognizerRenderCalculator : public CalculatorBase
{
public:
    GestureRecognizerRenderCalculator(){};
    // ~GestureRecognizerRenderCalculator() override{};

    static ::mediapipe::Status GetContract(CalculatorContract *cc);

    ::mediapipe::Status Open(CalculatorContext *cc) override;

    ::mediapipe::Status Process(CalculatorContext *cc) override;

private:
    void AddLabels(const NormalizedLandmarkList landmarks,
            float text_line_height, const std::string& text, RenderData* render_data);

    float get_Euclidean_DistanceAB(float a_x, float a_y, float b_x, float b_y)
    {
        float dist = std::pow(a_x - b_x, 2) + pow(a_y - b_y, 2);
        return std::sqrt(dist);
    }

    bool isThumbNearFirstFinger(NormalizedLandmark point1, NormalizedLandmark point2)
    {
        float distance = this->get_Euclidean_DistanceAB(point1.x(), point1.y(), point2.x(), point2.y());
        return distance < 0.1;
    }
};
REGISTER_CALCULATOR(GestureRecognizerRenderCalculator);

// Verifies input and output packet types
::mediapipe::Status GestureRecognizerRenderCalculator::GetContract(CalculatorContract *cc)
{
    // Checks if input stream has the kNormLandmarksTag
    RET_CHECK(cc->Inputs().HasTag(kNormLandmarksTag));
    // Set kNormLandmarksTag to receive a NormalizedLandmarkList as input
    cc->Inputs().Tag(kNormLandmarksTag).Set<NormalizedLandmarkList>();

    //// Checks if input stream has the normRectTag
    //RET_CHECK(cc->Inputs().HasTag(normRectTag));
    //// Set normRectTag to receive a NormalizedRect as input
    //cc->Inputs().Tag(normRectTag).Set<NormalizedRect>();

    // Check if output stream has tag kRenderDataTag
    RET_CHECK(cc->Outputs().HasTag(kRenderDataTag));
    // Set output stream to recognizedHandGesture string
    cc->Outputs().Tag(kRenderDataTag).Set<RenderData>();

    return ::mediapipe::OkStatus();
}

::mediapipe::Status GestureRecognizerRenderCalculator::Open(CalculatorContext *cc)
{
    // Must look into this carefully
    cc->SetOffset(TimestampDiff(0));
    return ::mediapipe::OkStatus();
}

::mediapipe::Status GestureRecognizerRenderCalculator::Process(CalculatorContext *cc)
{
    //const auto rect = &(cc->Inputs().Tag(normRectTag).Get<NormalizedRect>());

    //float width = rect->width();
    //float height = rect->height();

    //// string variable to hold gesture text
    std::string gesture_text;

    //if (width < 0.01 || height < 0.01)
    //{
        //gesture_text = new std::string("Finding hand");
        //LOG(INFO) << "No hand detected";

        //cc->Outputs().Tag(kRenderDataTag).Add(gesture_text, cc->InputTimestamp());

        //return ::mediapipe::OkStatus();
    //}

    const auto &landmarkList = cc->Inputs().Tag(kNormLandmarksTag).Get<NormalizedLandmarkList>();
    // const auto &landmark_vector = cc->Inputs().Tag(kNormLandmarksTag).Get<std::vector<NormalizedLandmarkList>>();
    RET_CHECK_GT(landmarkList.landmark_size(), 0) << "Input landmark vector is empty";

    // for (int i = 0; i < landmarkList.landmark_size(); ++i)
    // {
    //     const mediapipe::NormalizedLandmark &landmark = landmarkList.landmark(i);
    //     LOG(INFO) << "x coordinate: " << landmark.x();
    //     LOG(INFO) << "y coordinate: " << landmark.y();
    //     LOG(INFO) << "z coordinate: " << landmark.z();
    // }
    landmarkList.PrintDebugString();

    // finger states
    bool thumbIsOpen = false;
    bool firstFingerIsOpen = false;
    bool secondFingerIsOpen = false;
    bool thirdFingerIsOpen = false;
    bool fourthFingerIsOpen = false;
    //

    float pseudoFixKeyPoint = landmarkList.landmark(2).x();
    if (landmarkList.landmark(3).x() < pseudoFixKeyPoint && landmarkList.landmark(4).x() < pseudoFixKeyPoint)
    {
        thumbIsOpen = true;
    }

    pseudoFixKeyPoint = landmarkList.landmark(6).y();
    if (landmarkList.landmark(7).y() < pseudoFixKeyPoint && landmarkList.landmark(8).y() < pseudoFixKeyPoint)
    {
        firstFingerIsOpen = true;
    }

    pseudoFixKeyPoint = landmarkList.landmark(10).y();
    if (landmarkList.landmark(11).y() < pseudoFixKeyPoint && landmarkList.landmark(12).y() < pseudoFixKeyPoint)
    {
        secondFingerIsOpen = true;
    }

    pseudoFixKeyPoint = landmarkList.landmark(14).y();
    if (landmarkList.landmark(15).y() < pseudoFixKeyPoint && landmarkList.landmark(16).y() < pseudoFixKeyPoint)
    {
        thirdFingerIsOpen = true;
    }

    pseudoFixKeyPoint = landmarkList.landmark(18).y();
    if (landmarkList.landmark(19).y() < pseudoFixKeyPoint && landmarkList.landmark(20).y() < pseudoFixKeyPoint)
    {
        fourthFingerIsOpen = true;
    }

    // Hand gesture recognition
    if (thumbIsOpen && firstFingerIsOpen && secondFingerIsOpen && thirdFingerIsOpen && fourthFingerIsOpen)
    {
        LOG(INFO) << "FIVE!";
        gesture_text = std::string("FIVE");
    }
    else if (!thumbIsOpen && firstFingerIsOpen && secondFingerIsOpen && thirdFingerIsOpen && fourthFingerIsOpen)
    {
        LOG(INFO) << "FOUR!";
        gesture_text = std::string("FOUR");
    }
    else if (thumbIsOpen && firstFingerIsOpen && secondFingerIsOpen && !thirdFingerIsOpen && !fourthFingerIsOpen)
    {
        LOG(INFO) << "THREE!";
        gesture_text = std::string("THREE");
    }
    else if (thumbIsOpen && firstFingerIsOpen && !secondFingerIsOpen && !thirdFingerIsOpen && !fourthFingerIsOpen)
    {
        LOG(INFO) << "TWO!";
        gesture_text = std::string("TWO");
    }
    else if (!thumbIsOpen && firstFingerIsOpen && !secondFingerIsOpen && !thirdFingerIsOpen && !fourthFingerIsOpen)
    {
        LOG(INFO) << "ONE!";
        gesture_text = std::string("ONE");
    }
    else if (!thumbIsOpen && firstFingerIsOpen && secondFingerIsOpen && !thirdFingerIsOpen && !fourthFingerIsOpen)
    {
        LOG(INFO) << "TWO SURE!";
        gesture_text = std::string("TWO SURE");
    }
    else if (!thumbIsOpen && firstFingerIsOpen && !secondFingerIsOpen && !thirdFingerIsOpen && fourthFingerIsOpen)
    {
        LOG(INFO) << "ROCK!";
        gesture_text = std::string("ROCK");
    }
    else if (thumbIsOpen && firstFingerIsOpen && !secondFingerIsOpen && !thirdFingerIsOpen && fourthFingerIsOpen)
    {
        LOG(INFO) << "SPIDERMAN!";
        gesture_text = std::string("SPIDERMAN");
    }
    else if (!thumbIsOpen && !firstFingerIsOpen && !secondFingerIsOpen && !thirdFingerIsOpen && !fourthFingerIsOpen)
    {
        LOG(INFO) << "FIST!";
        gesture_text = std::string("FIST");
    }
    else if (!firstFingerIsOpen && secondFingerIsOpen && thirdFingerIsOpen && fourthFingerIsOpen && this->isThumbNearFirstFinger(landmarkList.landmark(4), landmarkList.landmark(8)))
    {
        LOG(INFO) << "OK!";
        gesture_text = std::string("OK");
    }
    else
    {
        LOG(INFO) << "Finger States: " << thumbIsOpen << firstFingerIsOpen << secondFingerIsOpen << thirdFingerIsOpen << fourthFingerIsOpen;
        LOG(INFO) << "___";
        gesture_text = std::string("____");
    }

    auto render_data = absl::make_unique<RenderData>();

    AddLabels(landmarkList, 10, gesture_text, render_data.get());

    // We set output stream to recognized hand gesture text
    cc->Outputs()
        .Tag(kRenderDataTag)
        .Add(render_data.release(), cc->InputTimestamp());

    return ::mediapipe::OkStatus();
}

void GestureRecognizerRenderCalculator::AddLabels(
        const NormalizedLandmarkList landmarks,
        float text_line_height,
        const std::string& display_text,
        RenderData* render_data) {
    auto* label_annotation = render_data->add_render_annotations();
    label_annotation->set_thickness(1);
      label_annotation->mutable_color()->set_r(255);
      label_annotation->mutable_color()->set_g(0);
      label_annotation->mutable_color()->set_b(0);

    float left_x = 1.0;
    float left_y = 1.0;

    for(int i=0; i < 21; ++i) {
        auto x = landmarks.landmark(i).x();
        if(left_x > x) {
            left_x = x;
        }
        auto y = landmarks.landmark(i).y();
        if(left_y > y) {
            left_y = y;
        }
    }

    //LOG(INFO) << "x: " << left_x;
    //LOG(INFO) << "y: " << left_y;

    auto* text = label_annotation->mutable_text();
    text->set_display_text(display_text);
    text->set_font_height(50);
    //TODO: fix the location
    text->set_left((int)(left_x*480));
    //TODO: fix the location
    text->set_baseline((int)(left_y*1280));
    text->set_font_face(5);
}

} // namespace mediapipe
