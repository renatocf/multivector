/******************************************************************************/
/*  MultiVector - Linearized multi-dimensional vectors with a simple syntax   */
/*  Copyright (C) 2018 Renato Cordeiro Ferreira                               */
/*                                                                            */
/*  This program is free software: you can redistribute it and/or modify      */
/*  it under the terms of the GNU General Public License as published by      */
/*  the Free Software Foundation, either version 3 of the License, or         */
/*  (at your option) any later version.                                       */
/*                                                                            */
/*  This program is distributed in the hope that it will be useful,           */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/*  GNU General Public License for more details.                              */
/*                                                                            */
/*  You should have received a copy of the GNU General Public License         */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.     */
/******************************************************************************/

// Standard headers
#include <algorithm>
#include <functional>
#include <limits>
#include <vector>

// External headers
#include "gmock/gmock.h"

// Tested header
#include "multivector/multivector.hpp"

/*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
/*----------------------------------------------------------------------------*/
/*                             USING DECLARATIONS                             */
/*----------------------------------------------------------------------------*/
/*////////////////////////////////////////////////////////////////////////////*/

using ::testing::Eq;
using ::testing::Ne;
using ::testing::Lt;
using ::testing::Le;
using ::testing::Gt;
using ::testing::Ge;

using ::testing::DoubleEq;

using multivector::multivector_t;
using multivector::view_t;
using multivector::range_t;

#define DOUBLE(X) static_cast<double>(X)

/*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
/*----------------------------------------------------------------------------*/
/*                                  FIXTURES                                  */
/*----------------------------------------------------------------------------*/
/*////////////////////////////////////////////////////////////////////////////*/

struct ACubeMultiVector : public testing::Test {
  multivector_t<int> cube { 3, 3, 3 };
};

struct ARange : public testing::Test {
  range_t range { 23, 42 };
};

struct ACubeView : public testing::Test {
  multivector_t<int> cube { 3, 3, 3 };
  view_t<int> cube_view {
    cube, {}, { range_t(0, 3), range_t(0, 3), range_t(0, 3) } };
};

struct APlanView : public testing::Test {
  multivector_t<int> cube { 3, 3, 3 };
  view_t<int> plan_view {
    cube, { 0 }, { range_t(0, 3), range_t(0, 3) } };
};

struct ALineView : public testing::Test {
  multivector_t<int> cube { 3, 3, 3 };
  view_t<int> line_view {
    cube, { 0, 0 }, { range_t(0, 3) } };
};

struct APointView : public testing::Test {
  multivector_t<int> cube { 3, 3, 3 };
  view_t<int> point_view { cube, { 0, 0, 0 }, {} };
};

/*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
/*----------------------------------------------------------------------------*/
/*                                SIMPLE TESTS                                */
/*----------------------------------------------------------------------------*/
/*////////////////////////////////////////////////////////////////////////////*/

/*----------------------------------------------------------------------------*/
/*                                MULTIVECTOR                                 */
/*----------------------------------------------------------------------------*/

TEST(MultiVector, CanBeDefaultInitialized) {
  multivector_t<int> null;
  ASSERT_THAT(null.num_dimensions(), Eq(0));
}

/*----------------------------------------------------------------------------*/

TEST(MultiVector, CanBeInitializedWithThreeDimensionsViaInitializerList) {
  multivector_t<int> cube { 1, 2, 3 };
  ASSERT_THAT(cube.num_dimensions(), Eq(3));
  ASSERT_THAT(cube.dimension_size(0), Eq(1));
  ASSERT_THAT(cube.dimension_size(1), Eq(2));
  ASSERT_THAT(cube.dimension_size(2), Eq(3));
  ASSERT_THAT(cube.buffer_size(), Eq(6));
}

/*----------------------------------------------------------------------------*/

TEST(MultiVector, CanBeInitializedWithThreeDimensionsViaConstructor) {
  multivector_t<int> cube({ 1, 2, 3 });
  ASSERT_THAT(cube.num_dimensions(), Eq(3));
  ASSERT_THAT(cube.dimension_size(0), Eq(1));
  ASSERT_THAT(cube.dimension_size(1), Eq(2));
  ASSERT_THAT(cube.dimension_size(2), Eq(3));
  ASSERT_THAT(cube.buffer_size(), Eq(6));
}

/*----------------------------------------------------------------------------*/

TEST(MultiVector, CanBeInitializedWithThreeDimensionsAndNonDefaultValue) {
  multivector_t<int> cube(42, { 1, 2, 3 });
  ASSERT_THAT(cube.num_dimensions(), Eq(3));
  ASSERT_THAT(cube.dimension_size(0), Eq(1));
  ASSERT_THAT(cube.dimension_size(1), Eq(2));
  ASSERT_THAT(cube.dimension_size(2), Eq(3));
  ASSERT_THAT(cube.buffer_size(), Eq(6));
}

/*----------------------------------------------------------------------------*/
/*                                   RANGE                                    */
/*----------------------------------------------------------------------------*/

TEST(Range, CanBeDefaultInitialized) {
  range_t origin;
  ASSERT_TRUE(origin.begin == 0 && origin.end == 0);
}

/*----------------------------------------------------------------------------*/

TEST(Range, CanBeInitializedWithAPosition) {
  range_t from_origin(42);
  ASSERT_TRUE(from_origin.begin == 42 && from_origin.end == 43);
}

/*----------------------------------------------------------------------------*/

TEST(Range, CanBeInitializedWithANonZeroBeginAndEnd) {
  range_t segment(23, 42);
  ASSERT_TRUE(segment.begin == 23 && segment.end == 42);
}

/*----------------------------------------------------------------------------*/

TEST(Range, CanBeInitializedWithAnEqualNonZeroBeginAndEnd) {
  range_t point(42, 42);
  ASSERT_TRUE(point.begin == 42 && point.end == 42);
}

/*----------------------------------------------------------------------------*/

TEST(Range, DiesIfBeginIsBiggerThanEnd) {
  ASSERT_DEATH(range_t from_origin(42, 23), "begin <= end");
}

/*----------------------------------------------------------------------------*/
/*                              MULTIVECTOR VIEW                              */
/*----------------------------------------------------------------------------*/

TEST(View, CanBeInitializedWithReferenceToContainerMultiVector) {
  multivector_t<int> cube { 1, 2, 3 };
  view_t<int> null_view(cube);

  ASSERT_THAT(&null_view.container(), Eq(&cube));
}

/*----------------------------------------------------------------------------*/

TEST(View, CanBeInitializedWithAsManyFullRangesAsContainerDimensions) {
  multivector_t<int> cube { 1, 2, 3 };
  std::vector<range_t> ranges { range_t(0, 1), range_t(0, 2), range_t(0, 3) };

  view_t<int> plan_view(cube, ranges);

  ASSERT_THAT(&plan_view.container(), Eq(&cube));
}

/*----------------------------------------------------------------------------*/

TEST(View, DiesIfInitializedWithLessFullRangesThanContainerDimensions) {
  multivector_t<int> cube { 1, 2, 3 };
  std::vector<range_t> ranges { range_t(0, 2), range_t(0, 3) };

  ASSERT_DEATH(view_t<int> plan_view(cube, ranges), "check_num_dimensions");
}

/*----------------------------------------------------------------------------*/

TEST(View, DiesIfInitializedWithMoreFullRangesThanContainerDimensions) {
  multivector_t<int> cube { 1, 2, 3 };
  std::vector<range_t> ranges {
    range_t(0, 1), range_t(0, 2), range_t(0, 3), range_t(0, 4)
  };

  ASSERT_DEATH(view_t<int> plan_view(cube, ranges), "check_num_dimensions");
}

/*----------------------------------------------------------------------------*/

TEST(View, DiesIfFullRangeEndIsBiggerThanContainerDimension) {
  multivector_t<int> cube { 1, 2, 3 };
  std::vector<range_t> ranges { range_t(0, 1), range_t(0, 2), range_t(0, 4) };

  ASSERT_DEATH(view_t<int> plan_view(cube, ranges), "check_ranges");
}

/*----------------------------------------------------------------------------*/

TEST(View, VerifiesFullRangesFromRightToLeft) {
  multivector_t<int> cube { 3, 2, 1 };
  std::vector<size_t> indices { 0 };
  std::vector<range_t> ranges { range_t(0, 3), range_t(0, 2) };

  ASSERT_DEATH(view_t<int> plan_view(cube, indices, ranges), "check_ranges");
}

/*----------------------------------------------------------------------------*/

TEST(View, CanBeInitializedWithAsManyPartialRangesAsContainerDimensions) {
  multivector_t<int> cube { 1, 2, 3 };
  std::vector<range_t> ranges { range_t(0, 1), range_t(1, 2), range_t(2, 3) };

  view_t<int> plan_view(cube, ranges);

  ASSERT_THAT(&plan_view.container(), Eq(&cube));
}

/*----------------------------------------------------------------------------*/

TEST(View, DiesIfInitializedWithLessPartialRangesThanContainerDimensions) {
  multivector_t<int> cube { 1, 2, 3 };
  std::vector<range_t> ranges { range_t(1, 2), range_t(2, 3) };

  ASSERT_DEATH(view_t<int> plan_view(cube, ranges), "check_num_dimensions");
}

/*----------------------------------------------------------------------------*/

TEST(View, DiesIfInitializedWithMorePartialRangesThanContainerDimensions) {
  multivector_t<int> cube { 1, 2, 3 };
  std::vector<range_t> ranges {
    range_t(0, 1), range_t(1, 2), range_t(2, 3), range_t(3, 4)
  };

  ASSERT_DEATH(view_t<int> plan_view(cube, ranges), "check_num_dimensions");
}

/*----------------------------------------------------------------------------*/

TEST(View, DiesIfPartialRangeEndIsBiggerThanContainerDimensions) {
  multivector_t<int> cube { 1, 2, 3 };
  std::vector<size_t> indices { 0 };
  std::vector<range_t> ranges { range_t(1, 2), range_t(3, 4) };

  ASSERT_DEATH(view_t<int> plan_view(cube, indices, ranges), "check_ranges");
}

/*----------------------------------------------------------------------------*/

TEST(View, VerifiesPartialRangesFromRightToLeft) {
  multivector_t<int> cube { 3, 2, 1 };
  std::vector<size_t> indices { 0 };
  std::vector<range_t> ranges { range_t(2, 3), range_t(1, 2) };

  ASSERT_DEATH(view_t<int> plan_view(cube, indices, ranges), "check_ranges");
}

/*----------------------------------------------------------------------------*/

TEST(View, CanBeInitializedWithAsManyIndicesAsContainerDimensions) {
  multivector_t<int> cube { 1, 2, 3 };
  std::vector<size_t> indices { 0, 0, 0 };

  view_t<int> plan_view(cube, indices, {});

  ASSERT_THAT(&plan_view.container(), Eq(&cube));
}

/*----------------------------------------------------------------------------*/

TEST(View, DiesIfInitializedWithLessIndicesThanContainerDimensions) {
  multivector_t<int> cube { 1, 2, 3 };
  std::vector<size_t> indices { 0, 0 };

  ASSERT_DEATH(view_t<int> plan_view(cube, indices, {}),
               "check_num_dimensions");
}

/*----------------------------------------------------------------------------*/

TEST(View, DiesIfInitializedWithMoreIndicesThanContainerDimensions) {
  multivector_t<int> cube { 1, 2, 3 };
  std::vector<size_t> indices { 0, 0, 0, 0 };

  ASSERT_DEATH(view_t<int> plan_view(cube, indices, {}),
               "check_num_dimensions");
}

/*----------------------------------------------------------------------------*/

TEST(View, DiesIfPositionIsBiggerThanContainerDimension) {
  multivector_t<int> cube { 1, 2, 3 };
  std::vector<size_t> indices { 2 };
  std::vector<range_t> ranges { range_t(1, 2), range_t(2, 3) };

  ASSERT_DEATH(view_t<int> plan_view(cube, indices, ranges), "check_indices");
}

/*----------------------------------------------------------------------------*/

TEST(View, VerifiesIndicesFromRightToLeft) {
  multivector_t<int> cube { 3, 2, 1 };
  std::vector<size_t> indices { 2, 3 };
  std::vector<range_t> ranges { range_t(0, 0) };

  ASSERT_DEATH(view_t<int> plan_view(cube, indices, ranges), "check_indices");
}

/*----------------------------------------------------------------------------*/

TEST(View, CanBeInitializedWithMoreIndicesThanFullRanges) {
  multivector_t<int> cube { 1, 2, 3 };
  std::vector<size_t> indices { 0, 0 };
  std::vector<range_t> ranges { range_t(0, 3) };

  view_t<int> plan_view(cube, indices, ranges);

  ASSERT_THAT(&plan_view.container(), Eq(&cube));
}

/*----------------------------------------------------------------------------*/

TEST(View, CanBeInitializedWithMoreIndicesThanPartialRanges) {
  multivector_t<int> cube { 1, 2, 3 };
  std::vector<size_t> indices { 0, 0 };
  std::vector<range_t> ranges { range_t(2, 3) };

  view_t<int> plan_view(cube, indices, ranges);

  ASSERT_THAT(&plan_view.container(), Eq(&cube));
}

/*----------------------------------------------------------------------------*/

TEST(View, CanBeInitializedWithMoreFullRangesThanIndices) {
  multivector_t<int> cube { 1, 2, 3 };
  std::vector<size_t> indices { 0 };
  std::vector<range_t> ranges { range_t(0, 2), range_t(0, 3) };

  view_t<int> plan_view(cube, indices, ranges);

  ASSERT_THAT(&plan_view.container(), Eq(&cube));
}

/*----------------------------------------------------------------------------*/

TEST(View, CanBeInitializedWithMorePartialRangesThanIndices) {
  multivector_t<int> cube { 1, 2, 3 };
  std::vector<size_t> indices { 0 };
  std::vector<range_t> ranges { range_t(1, 2), range_t(2, 3) };

  view_t<int> plan_view(cube, indices, ranges);

  ASSERT_THAT(&plan_view.container(), Eq(&cube));
}

/*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
/*----------------------------------------------------------------------------*/
/*                             TESTS WITH FIXTURE                             */
/*----------------------------------------------------------------------------*/
/*////////////////////////////////////////////////////////////////////////////*/

/*----------------------------------------------------------------------------*/
/*                              CUBE MULTIVECTOR                              */
/*----------------------------------------------------------------------------*/

TEST_F(ACubeMultiVector, HasRightNumberOfDimensions) {
  ASSERT_THAT(cube.num_dimensions(), Eq(3));
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeMultiVector, HasRightBufferSize) {
  ASSERT_THAT(cube.buffer_size(), Eq(27));
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeMultiVector, HasCorrectDimensionSizes) {
  EXPECT_THAT(cube.dimension_size(0), Eq(3));
  EXPECT_THAT(cube.dimension_size(1), Eq(3));
  EXPECT_THAT(cube.dimension_size(2), Eq(3));
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeMultiVector, DiesGettingSizeOfNonExistentDimension) {
  ASSERT_DEATH(cube.dimension_size(3), "");
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeMultiVector,
       IsEqualToAnotherMultiVectorWithSameDimensionsAndValues) {
  multivector_t<int> another_cube { 3, 3, 3 };
  EXPECT_TRUE(cube == another_cube);
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeMultiVector, IsDifferentFromAMultiVectorWithLessDimensions) {
  multivector_t<int> plan { 3, 3 };
  EXPECT_TRUE(cube != plan);
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeMultiVector, IsDifferentFromAMultiVectorWithMoreDimensions) {
  multivector_t<int> hypercube { 3, 3, 3, 3 };
  EXPECT_TRUE(cube != hypercube);
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeMultiVector,
       IsDifferentFromAMultiVectorWithSameDimensionsButDifferentValues) {
  multivector_t<int> another_cube { 3, 3, 3 };
  std::vector<size_t> indices { 0, 0, 0 };

  another_cube[indices] = 42;

  EXPECT_TRUE(cube != another_cube);
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeMultiVector, AllowsAccessToItsBufferOffset) {
  ASSERT_THAT(cube.buffer_offset({ 0, 0, 0 }), Eq(0));
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeMultiVector,
       DiesIfGeneratingBufferOffsetWithLessIndicesThanDimensions) {
  ASSERT_DEATH(cube.buffer_offset({ 0, 0 }),
               "check_as_many_indices_as_dimensions");
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeMultiVector,
       DiesIfGeneratingBufferOffsetWithMoreIndicesThanDimensions) {
  ASSERT_DEATH(cube.buffer_offset({ 0, 0, 0, 0 }),
               "check_as_many_indices_as_dimensions");
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeMultiVector,
       DiesIfGeneratingBufferOffsetFromIndexOutOfBoundInFirstDimension) {
  std::vector<size_t> indices { 4, 0, 0 };
  EXPECT_DEATH(cube.buffer_offset(indices), "check_index");
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeMultiVector,
       DiesIfGeneratingBufferOffsetFromIndexOutOfBoundInSecondDimension) {
  std::vector<size_t> indices { 0, 4, 0 };
  EXPECT_DEATH(cube.buffer_offset(indices), "check_index");
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeMultiVector,
       DiesGeneratingBufferOffsetFromIndexOutOfBoundInThirdDimension) {
  std::vector<size_t> indices { 0, 0, 4 };
  EXPECT_DEATH(cube.buffer_offset(indices), "check_index");
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeMultiVector, ConvertsFromIndicesToBufferOffsetCorrectly) {
  for (size_t i = 0; i < 3; i++)
    for (size_t j = 0; j < 3; j++)
      for (size_t k = 0; k < 3; k++)
        EXPECT_THAT(cube.buffer_offset({ i, j, k }), Eq(i*9 + j*3 + k));
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeMultiVector, AllowsAccessingFieldsByBufferOffset) {
  for (size_t i = 0; i < 3; i++) {
    for (size_t j = 0; j < 3; j++) {
      for (size_t k = 0; k < 3; k++) {
        std::vector<size_t> indices { i, j, k };
        auto offset = cube.buffer_offset(indices);
        EXPECT_THAT(cube.buffer_value(offset), Eq(0));
      }
    }
  }
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeMultiVector, AllowsChangingItsFieldsByBufferOffset) {
  for (size_t i = 0; i < 3; i++) {
    for (size_t j = 0; j < 3; j++) {
      for (size_t k = 0; k < 3; k++) {
        std::vector<size_t> indices { i, j, k };
        auto offset = cube.buffer_offset(indices);
        cube.buffer_value(offset) = i*9 + j*3 + k;
        EXPECT_THAT(cube.buffer_value(offset), Eq(i*9 + j*3 + k));
      }
    }
  }
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeMultiVector, DiesIfAccessingBufferOffsetOutOfBound) {
  std::vector<size_t> indices {
    cube.dimension_size(0), cube.dimension_size(1), cube.dimension_size(2) };

  auto offset = std::accumulate(
      std::begin(indices), std::end(indices), 1, std::multiplies<>());

  EXPECT_DEATH(cube.buffer_value(offset) = 42, "check_offset");
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeMultiVector, DiesIfChangingBufferOffsetOutOfBound) {
  std::vector<size_t> indices {
    cube.dimension_size(0), cube.dimension_size(1), cube.dimension_size(2) };

  auto offset = std::accumulate(
      std::begin(indices), std::end(indices), 1, std::multiplies<>());

  EXPECT_DEATH(cube.buffer_value(offset), "check_offset");
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeMultiVector, GeneratesAPlanViewByAccessingOneJointIndex) {
  view_t<int> plan_view(cube, { 0 }, { range_t(0, 3), range_t(0, 3) });

  std::vector<size_t> indices { 0 };
  ASSERT_TRUE(cube[indices] == plan_view);
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeMultiVector, GeneratesALineViewByAccessingTwoJointIndices) {
  view_t<int> line_view(cube, { 0, 0 }, { range_t(0, 3) });

  std::vector<size_t> indices { 0, 0 };
  ASSERT_TRUE(cube[indices] == line_view);
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeMultiVector, GeneratesAPointViewByAccessingThreeJointIndices) {
  view_t<int> point_view(cube, { 0, 0, 0 }, {});

  std::vector<size_t> indices { 0, 0, 0 };
  ASSERT_TRUE(cube[indices] == point_view);
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeMultiVector, DiesTryingToGenerateAViewWithZeroIndices) {
  std::vector<size_t> indices {};
  ASSERT_DEATH(cube[indices], "check_num_indices");
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeMultiVector,
       DiesTryingToGenerateAViewWithMoreJointIndicesThanRanges) {
  std::vector<size_t> indices { 0, 0, 0, 0 };
  ASSERT_DEATH(cube[indices], "check_num_indices");
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeMultiVector, GeneratesAPlanViewByAccessingOneSeparatedIndex) {
  view_t<int> plan_view(cube, { 0 }, { range_t(0, 3), range_t(0, 3) });

  ASSERT_TRUE(cube[0] == plan_view);
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeMultiVector, GeneratesALineViewByAccessingTwoSeparatedIndices) {
  view_t<int> line_view(cube, { 0, 0 }, { range_t(0, 3) });

  ASSERT_TRUE(cube[0][0] == line_view);
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeMultiVector, GeneratesAPointViewByAccessingThreeSeparatedIndices) {
  view_t<int> point_view(cube, { 0, 0, 0 }, {});

  ASSERT_TRUE(cube[0][0][0] == point_view);
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeMultiVector,
       DiesTryingToGenerateAViewWithMoreSeparatedIndicesThanRanges) {
  ASSERT_DEATH(cube[0][0][0][0], "check_remaining_ranges");
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeMultiVector, AllowsAccessingFieldsByJointIndices) {
  for (size_t i = 0; i < 3; i++) {
    for (size_t j = 0; j < 3; j++) {
      for (size_t k = 0; k < 3; k++) {
        std::vector<size_t> indices { i, j, k };
        EXPECT_THAT(cube[indices], Eq(0));
      }
    }
  }
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeMultiVector, AllowsChangingItsFieldsByJointIndices) {
  for (size_t i = 0; i < 3; i++) {
    for (size_t j = 0; j < 3; j++) {
      for (size_t k = 0; k < 3; k++) {
        std::vector<size_t> indices { i, j, k };
        cube[indices] = i*9 + j*3 + k;
        EXPECT_THAT(cube[indices], Eq(i*9 + j*3 + k));
      }
    }
  }
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeMultiVector,
       DiesChangingIndexOutOfBoundInFirstDimensionWithJointIndices) {
  std::vector<size_t> indices { 4, 0, 0 };
  EXPECT_DEATH(cube[indices] = 42, "check_indices");
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeMultiVector,
       DiesChangingIndexOutOfBoundInSecondDimensionWithJointIndices) {
  std::vector<size_t> indices { 0, 4, 0 };
  EXPECT_DEATH(cube[indices] = 42, "check_indices");
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeMultiVector,
       DiesChangingIndexOutOfBoundInThirdDimensionWithJointIndices) {
  std::vector<size_t> indices { 0, 0, 4 };
  EXPECT_DEATH(cube[indices] = 42, "check_indices");
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeMultiVector, AllowsAccessingFieldsBySeparatedIndices) {
  for (size_t i = 0; i < 3; i++) {
    for (size_t j = 0; j < 3; j++) {
      for (size_t k = 0; k < 3; k++) {
        EXPECT_THAT(cube[i][j][k], Eq(0));
      }
    }
  }
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeMultiVector, AllowsChangingItsFieldsBySeparatedIndices) {
  for (size_t i = 0; i < 3; i++) {
    for (size_t j = 0; j < 3; j++) {
      for (size_t k = 0; k < 3; k++) {
        cube[i][j][k] = i*9 + j*3 + k;
        EXPECT_THAT(cube[i][j][k], Eq(i*9 + j*3 + k));
      }
    }
  }
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeMultiVector,
       ChangesAllCorrectPositionsOfContainerWhenChangingBySeparatedIndices) {
  for (size_t i = 0; i < 3; i++) {
    for (size_t j = 0; j < 3; j++) {
      for (size_t k = 0; k < 3; k++) {
        cube[i][j][k] = i*9 + j*3 + k + 1;
      }
    }
  }

  for (size_t i = 0; i < 3; i++) {
    for (size_t j = 0; j < 3; j++) {
      for (size_t k = 0; k < 3; k++) {
        EXPECT_THAT(cube[i][j][k], Ne(0));
      }
    }
  }
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeMultiVector,
       DiesChangingIndexOutOfBoundInFirstDimensionWithSeparatedIndices) {
  EXPECT_DEATH(cube[4][0][0] = 42, "check_indices");
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeMultiVector,
       DiesChangingIndexOutOfBoundInSecondDimensionWithSeparatedIndices) {
  EXPECT_DEATH(cube[0][4][0] = 42, "check_indices");
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeMultiVector,
       DiesChangingIndexOutOfBoundInThirdDimensionWithSeparatedIndices) {
  EXPECT_DEATH(cube[0][0][4] = 42, "check_indices");
}

/*----------------------------------------------------------------------------*/
/*                                   RANGE                                    */
/*----------------------------------------------------------------------------*/

TEST_F(ARange, IsEqualityComparable) {
  ASSERT_TRUE(range == range_t(23, 42));
  ASSERT_TRUE(range != range_t(13, 29));
}

/*----------------------------------------------------------------------------*/
/*                           CUBE MULTIVECTOR VIEW                            */
/*----------------------------------------------------------------------------*/

TEST_F(ACubeView, HasRightNumberOfDimensions) {
  ASSERT_THAT(cube_view.num_dimensions(), Eq(3));
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeView, HasCorrectRanges) {
  EXPECT_TRUE(cube_view.dimension_range(0) == range_t(0, 3));
  EXPECT_TRUE(cube_view.dimension_range(1) == range_t(0, 3));
  EXPECT_TRUE(cube_view.dimension_range(2) == range_t(0, 3));
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeView, DiesGettingRangeOfNonExistentDimension) {
  ASSERT_DEATH(cube_view.dimension_range(3), "check_dimension");
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeView, IsEqualToAnotherViewWithSameRangesAndValues) {
  multivector_t<int> another_cube { 3, 3, 3 };
  view_t<int> another_cube_view {
    another_cube, {}, { range_t(0, 3), range_t(0, 3), range_t(0, 3) } };

  EXPECT_TRUE(cube_view == another_cube_view);
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeView, IsDifferentFromAViewWithLessDimensions) {
  view_t<int> plan_view { cube, { 0 }, { range_t(0, 3), range_t(0, 3) } };

  EXPECT_TRUE(cube_view != plan_view);
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeView, IsDifferentFromAViewWithSameDimensionsButDifferentValues) {
  multivector_t<int> another_cube { 3, 3, 3 };
  view_t<int> another_cube_view {
    another_cube, {}, { range_t(0, 3), range_t(0, 3), range_t(0, 3) } };

  std::vector<size_t> indices { 0, 0, 0 };
  another_cube[indices] = 42;

  EXPECT_TRUE(cube_view != another_cube_view);
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeView, GeneratesAPlanViewByAccessingOneJointIndex) {
  view_t<int> plan_view(cube, { 0 }, { range_t(0, 3), range_t(0, 3) });

  std::vector<size_t> indices { 0 };
  ASSERT_TRUE(cube_view[indices] == plan_view);
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeView, GeneratesALineViewByAccessingTwoJointIndices) {
  view_t<int> line_view(cube, { 0, 0 }, { range_t(0, 3) });

  std::vector<size_t> indices { 0, 0 };
  ASSERT_TRUE(cube_view[indices] == line_view);
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeView, GeneratesAPointViewByAccessingThreeJointIndices) {
  view_t<int> point_view(cube, { 0, 0, 0 }, {});

  std::vector<size_t> indices { 0, 0, 0 };
  ASSERT_TRUE(cube_view[indices] == point_view);
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeView, DiesTryingToGenerateAViewWithZeroIndices) {
  std::vector<size_t> indices {};
  ASSERT_DEATH(cube_view[indices], "check_num_indices");
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeView, DiesTryingToGenerateAViewWithMoreJointIndicesThanRanges) {
  std::vector<size_t> indices { 0, 0, 0, 0 };
  ASSERT_DEATH(cube_view[indices], "check_num_indices");
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeView, GeneratesAPlanViewByAccessingOneSeparatedIndex) {
  view_t<int> plan_view(cube, { 0 }, { range_t(0, 3), range_t(0, 3) });

  ASSERT_TRUE(cube_view[0] == plan_view);
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeView, GeneratesALineViewByAccessingTwoSeparatedIndices) {
  view_t<int> line_view(cube, { 0, 0 }, { range_t(0, 3) });

  ASSERT_TRUE(cube_view[0][0] == line_view);
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeView, GeneratesAPointViewByAccessingThreeSeparatedIndices) {
  view_t<int> point_view(cube, { 0, 0, 0 }, {});

  ASSERT_TRUE(cube_view[0][0][0] == point_view);
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeView, DiesTryingToGenerateAViewWithMoreSeparatedIndicesThanRanges) {
  ASSERT_DEATH(cube_view[0][0][0][0], "check_remaining_ranges");
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeView, AllowsAccessingFieldsByJointIndices) {
  for (size_t i = 0; i < 3; i++) {
    for (size_t j = 0; j < 3; j++) {
      for (size_t k = 0; k < 3; k++) {
        std::vector<size_t> indices { i, j, k };
        EXPECT_THAT(cube_view[indices], Eq(0));
      }
    }
  }
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeView, AllowsChangingItsFieldsByJointIndices) {
  for (size_t i = 0; i < 3; i++) {
    for (size_t j = 0; j < 3; j++) {
      for (size_t k = 0; k < 3; k++) {
        std::vector<size_t> indices { i, j, k };
        cube_view[indices] = i*9 + j*3 + k;
        EXPECT_THAT(cube_view[indices], Eq(i*9 + j*3 + k));
      }
    }
  }
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeView,
       ChangesAllCorrectPositionsOfContainerWhenChangingByJointIndices) {
  for (size_t i = 0; i < 3; i++) {
    for (size_t j = 0; j < 3; j++) {
      for (size_t k = 0; k < 3; k++) {
        std::vector<size_t> indices { i, j, k };
        cube_view[indices] = i*9 + j*3 + k + 1;
      }
    }
  }

  for (size_t i = 0; i < 3; i++) {
    for (size_t j = 0; j < 3; j++) {
      for (size_t k = 0; k < 3; k++) {
        std::vector<size_t> indices { i, j, k };
        EXPECT_THAT(cube[indices], Ne(0));
      }
    }
  }
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeView,
       DiesChangingIndexOutOfBoundInFirstDimensionWithJointIndices) {
  std::vector<size_t> indices { 4, 0, 0 };
  EXPECT_DEATH(cube_view[indices] = 42, "check_indices");
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeView,
       DiesChangingIndexOutOfBoundInSecondDimensionWithJointIndices) {
  std::vector<size_t> indices { 0, 4, 0 };
  EXPECT_DEATH(cube_view[indices] = 42, "check_indices");
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeView,
       DiesChangingIndexOutOfBoundInThirdDimensionWithJointIndices) {
  std::vector<size_t> indices { 0, 0, 4 };
  EXPECT_DEATH(cube_view[indices] = 42, "check_indices");
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeView, AllowsAccessingFieldsBySeparatedIndices) {
  for (size_t i = 0; i < 3; i++) {
    for (size_t j = 0; j < 3; j++) {
      for (size_t k = 0; k < 3; k++) {
        EXPECT_THAT(cube_view[i][j][k], Eq(0));
      }
    }
  }
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeView, AllowsChangingItsFieldsBySeparatedIndices) {
  for (size_t i = 0; i < 3; i++) {
    for (size_t j = 0; j < 3; j++) {
      for (size_t k = 0; k < 3; k++) {
        cube_view[i][j][k] = i*9 + j*3 + k;
        EXPECT_THAT(cube_view[i][j][k], Eq(i*9 + j*3 + k));
      }
    }
  }
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeView,
       ChangesAllCorrectPositionsOfContainerWhenChangingBySeparatedIndices) {
  for (size_t i = 0; i < 3; i++) {
    for (size_t j = 0; j < 3; j++) {
      for (size_t k = 0; k < 3; k++) {
        cube_view[i][j][k] = i*9 + j*3 + k + 1;
      }
    }
  }

  for (size_t i = 0; i < 3; i++) {
    for (size_t j = 0; j < 3; j++) {
      for (size_t k = 0; k < 3; k++) {
        EXPECT_THAT(cube[i][j][k], Ne(0));
      }
    }
  }
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeView,
       DiesChangingIndexOutOfBoundInFirstDimensionWithSeparatedIndices) {
  EXPECT_DEATH(cube_view[4][0][0] = 42, "check_indices");
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeView,
       DiesChangingIndexOutOfBoundInSecondDimensionWithSeparatedIndices) {
  EXPECT_DEATH(cube_view[0][4][0] = 42, "check_indices");
}

/*----------------------------------------------------------------------------*/

TEST_F(ACubeView,
       DiesChangingIndexOutOfBoundInThirdDimensionWithSeparatedIndices) {
  EXPECT_DEATH(cube_view[0][0][4] = 42, "check_indices");
}

/*----------------------------------------------------------------------------*/
/*                           PLAN MULTIVECTOR VIEW                            */
/*----------------------------------------------------------------------------*/

TEST_F(APlanView, HasRightNumberOfDimensions) {
  ASSERT_THAT(plan_view.num_dimensions(), Eq(2));
}

/*----------------------------------------------------------------------------*/

TEST_F(APlanView, HasCorrectRanges) {
  EXPECT_TRUE(plan_view.dimension_range(0) == range_t(0, 3));
  EXPECT_TRUE(plan_view.dimension_range(1) == range_t(0, 3));
}

/*----------------------------------------------------------------------------*/

TEST_F(APlanView, DiesGettingRangeOfNonExistentDimension) {
  ASSERT_DEATH(plan_view.dimension_range(2), "check_dimension");
}

/*----------------------------------------------------------------------------*/

TEST_F(APlanView, IsEqualToAnotherViewWithSameRangesAndValues) {
  multivector_t<int> another_cube { 3, 3, 3 };
  view_t<int> another_plan_view {
    another_cube, { 0 }, { range_t(0, 3), range_t(0, 3) } };

  EXPECT_TRUE(plan_view == another_plan_view);
}

/*----------------------------------------------------------------------------*/

TEST_F(APlanView, IsDifferentFromAViewWithLessDimensions) {
  view_t<int> line_view {
    cube, { 0, 0 }, { range_t(0, 3) } };

  EXPECT_TRUE(plan_view != line_view);
}

/*----------------------------------------------------------------------------*/

TEST_F(APlanView, IsDifferentFromAViewWithMoreDimensions) {
  view_t<int> cube_view {
    cube, {}, { range_t(0, 3), range_t(0, 3), range_t(0, 3) } };

  EXPECT_TRUE(plan_view != cube_view);
}

/*----------------------------------------------------------------------------*/

TEST_F(APlanView, IsDifferentFromAViewWithSameDimensionsButDifferentValues) {
  multivector_t<int> another_cube { 3, 3, 3 };
  view_t<int> another_plan_view {
    another_cube, { 0 }, { range_t(0, 3), range_t(0, 3) } };

  std::vector<size_t> indices { 0, 0, 0 };
  another_cube[indices] = 42;

  EXPECT_TRUE(plan_view != another_plan_view);
}

/*----------------------------------------------------------------------------*/

TEST_F(APlanView, GeneratesALineViewByAccessingOneJointIndex) {
  view_t<int> line_view(cube, { 0, 0 }, { range_t(0, 3) });

  std::vector<size_t> indices { 0 };
  ASSERT_TRUE(plan_view[indices] == line_view);
}

/*----------------------------------------------------------------------------*/

TEST_F(APlanView, GeneratesAPointViewByAccessingTwoJointIndices) {
  view_t<int> point_view(cube, { 0, 0, 0 }, {});

  std::vector<size_t> indices { 0, 0 };
  ASSERT_TRUE(plan_view[indices] == point_view);
}

/*----------------------------------------------------------------------------*/

TEST_F(APlanView, DiesTryingToGenerateAViewWithZeroIndices) {
  std::vector<size_t> indices {};
  ASSERT_DEATH(plan_view[indices], "check_num_indices");
}

/*----------------------------------------------------------------------------*/

TEST_F(APlanView, DiesTryingToGenerateAViewWithMoreJointIndicesThanRanges) {
  std::vector<size_t> indices { 0, 0, 0 };
  ASSERT_DEATH(plan_view[indices], "check_num_indices");
}

/*----------------------------------------------------------------------------*/

TEST_F(APlanView, GeneratesALineViewByAccessingOneSeparatedIndex) {
  view_t<int> line_view(cube, { 0, 0 }, { range_t(0, 3) });

  ASSERT_TRUE(plan_view[0] == line_view);
}

/*----------------------------------------------------------------------------*/

TEST_F(APlanView, GeneratesAPointViewByAccessingTwoSeparatedIndices) {
  view_t<int> point_view(cube, { 0, 0, 0 }, {});

  ASSERT_TRUE(plan_view[0][0] == point_view);
}

/*----------------------------------------------------------------------------*/

TEST_F(APlanView, DiesTryingToGenerateAViewWithMoreSeparatedIndicesThanRanges) {
  ASSERT_DEATH(plan_view[0][0][0], "check_remaining_ranges");
}

/*----------------------------------------------------------------------------*/

TEST_F(APlanView, AllowsAccessingFieldsByJointIndices) {
  for (size_t j = 0; j < 3; j++) {
    for (size_t k = 0; k < 3; k++) {
      std::vector<size_t> indices { j, k };
      EXPECT_THAT(plan_view[indices], Eq(0));
    }
  }
}

/*----------------------------------------------------------------------------*/

TEST_F(APlanView, AllowsChangingItsFieldsByJointIndices) {
  for (size_t j = 0; j < 3; j++) {
    for (size_t k = 0; k < 3; k++) {
      std::vector<size_t> indices { j, k };
      plan_view[indices] = j*3 + k;
      EXPECT_THAT(plan_view[indices], Eq(j*3 + k));
    }
  }
}

/*----------------------------------------------------------------------------*/

TEST_F(APlanView,
       ChangesAllCorrectPositionsOfContainerWhenChangingByJointIndices) {
  for (size_t j = 0; j < 3; j++) {
    for (size_t k = 0; k < 3; k++) {
      std::vector<size_t> indices { j, k };
      plan_view[indices] = j*3 + k + 1;
    }
  }

  for (size_t j = 0; j < 3; j++) {
    for (size_t k = 0; k < 3; k++) {
      std::vector<size_t> indices { 0, j, k };
      EXPECT_THAT(cube[indices], Ne(0));
    }
  }

  for (size_t i = 1; i < 3; i++) {
    for (size_t j = 0; j < 3; j++) {
      for (size_t k = 0; k < 3; k++) {
        std::vector<size_t> indices { i, j, k };
        EXPECT_THAT(cube[indices], Eq(0));
      }
    }
  }
}

/*----------------------------------------------------------------------------*/

TEST_F(APlanView,
       DiesChangingIndexOutOfBoundInFirstDimensionWithJointIndices) {
  std::vector<size_t> indices { 4, 0 };
  EXPECT_DEATH(plan_view[indices] = 42, "check_indices");
}

/*----------------------------------------------------------------------------*/

TEST_F(APlanView,
       DiesChangingIndexOutOfBoundInSecondDimensionWithJointIndices) {
  std::vector<size_t> indices { 0, 4 };
  EXPECT_DEATH(plan_view[indices] = 42, "check_indices");
}

/*----------------------------------------------------------------------------*/

TEST_F(APlanView, AllowsAccessingFieldsBySeparatedIndices) {
  for (size_t j = 0; j < 3; j++) {
    for (size_t k = 0; k < 3; k++) {
      EXPECT_THAT(plan_view[j][k], Eq(0));
    }
  }
}

/*----------------------------------------------------------------------------*/

TEST_F(APlanView, AllowsChangingItsFieldsBySeparatedIndices) {
  for (size_t j = 0; j < 3; j++) {
    for (size_t k = 0; k < 3; k++) {
      plan_view[j][k] = j*3 + k;
      EXPECT_THAT(plan_view[j][k], Eq(j*3 + k));
    }
  }
}

/*----------------------------------------------------------------------------*/

TEST_F(APlanView,
       ChangesAllCorrectPositionsOfContainerWhenChangingBySeparatedIndices) {
  for (size_t j = 0; j < 3; j++) {
    for (size_t k = 0; k < 3; k++) {
      plan_view[j][k] = j*3 + k + 1;
    }
  }

  for (size_t j = 0; j < 3; j++) {
    for (size_t k = 0; k < 3; k++) {
      EXPECT_THAT(cube[0][j][k], Ne(0));
    }
  }

  for (size_t i = 1; i < 3; i++) {
    for (size_t j = 0; j < 3; j++) {
      for (size_t k = 0; k < 3; k++) {
        EXPECT_THAT(cube[i][j][k], Eq(0));
      }
    }
  }
}

/*----------------------------------------------------------------------------*/

TEST_F(APlanView,
       DiesChangingIndexOutOfBoundInFirstDimensionWithSeparatedIndices) {
  EXPECT_DEATH(plan_view[4][0] = 42, "check_indices");
}

/*----------------------------------------------------------------------------*/

TEST_F(APlanView,
       DiesChangingIndexOutOfBoundInSecondDimensionWithSeparatedIndices) {
  EXPECT_DEATH(plan_view[0][4] = 42, "check_indices");
}

/*----------------------------------------------------------------------------*/
/*                           LINE MULTIVECTOR VIEW                            */
/*----------------------------------------------------------------------------*/

TEST_F(ALineView, HasRightNumberOfDimensions) {
  ASSERT_THAT(line_view.num_dimensions(), Eq(1));
}

/*----------------------------------------------------------------------------*/

TEST_F(ALineView, HasCorrectRanges) {
  EXPECT_TRUE(line_view.dimension_range(0) == range_t(0, 3));
}

/*----------------------------------------------------------------------------*/

TEST_F(ALineView, DiesGettingRangeOfNonExistentDimension) {
  ASSERT_DEATH(line_view.dimension_range(1), "check_dimension");
}

/*----------------------------------------------------------------------------*/

TEST_F(ALineView, IsEqualToAnotherViewWithSameRangesAndValues) {
  multivector_t<int> another_cube { 3, 3, 3 };
  view_t<int> another_line_view { another_cube, { 0, 0 }, { range_t(0, 3) } };

  EXPECT_TRUE(line_view == another_line_view);
}

/*----------------------------------------------------------------------------*/

TEST_F(ALineView, IsDifferentFromAViewWithLessDimensions) {
  view_t<int> point_view { cube, { 0, 0, 0 }, {} };

  EXPECT_TRUE(line_view != point_view);
}

/*----------------------------------------------------------------------------*/

TEST_F(ALineView, IsDifferentFromAViewWithMoreDimensions) {
  view_t<int> plan_view { cube, { 0 }, { range_t(0, 3), range_t(0, 3) } };

  EXPECT_TRUE(line_view != plan_view);
}

/*----------------------------------------------------------------------------*/

TEST_F(ALineView, IsDifferentFromAViewWithSameDimensionsButDifferentValues) {
  multivector_t<int> another_cube { 3, 3, 3 };
  view_t<int> another_line_view { another_cube, { 0, 0 }, { range_t(0, 3) } };

  std::vector<size_t> indices { 0, 0, 0 };
  another_cube[indices] = 42;

  EXPECT_TRUE(line_view != another_line_view);
}

/*----------------------------------------------------------------------------*/

TEST_F(ALineView, GeneratesAPointViewByAccessingOneJointIndex) {
  view_t<int> point_view(cube, { 0, 0, 0 }, {});

  std::vector<size_t> indices { 0 };
  ASSERT_TRUE(line_view[indices] == point_view);
}

/*----------------------------------------------------------------------------*/

TEST_F(ALineView, DiesTryingToGenerateAViewWithZeroIndices) {
  std::vector<size_t> indices {};
  ASSERT_DEATH(line_view[indices], "check_num_indices");
}

/*----------------------------------------------------------------------------*/

TEST_F(ALineView, DiesTryingToGenerateAViewWithMoreJointIndicesThanRanges) {
  std::vector<size_t> indices { 0, 0 };
  ASSERT_DEATH(line_view[indices], "check_num_indices");
}

/*----------------------------------------------------------------------------*/

TEST_F(ALineView, GeneratesAPointViewByAccessingOneSeparatedIndex) {
  view_t<int> point_view(cube, { 0, 0, 0 }, {});

  ASSERT_TRUE(line_view[0] == point_view);
}

/*----------------------------------------------------------------------------*/

TEST_F(ALineView, DiesTryingToGenerateAViewWithMoreSeparatedIndicesThanRanges) {
  ASSERT_DEATH(line_view[0][0], "check_remaining_ranges");
}

/*----------------------------------------------------------------------------*/

TEST_F(ALineView, AllowsAccessingFieldsByJointIndices) {
  for (size_t k = 0; k < 3; k++) {
    std::vector<size_t> indices { k };
    EXPECT_THAT(line_view[indices], Eq(0));
  }
}

/*----------------------------------------------------------------------------*/

TEST_F(ALineView, AllowsChangingItsFieldsByJointIndices) {
  for (size_t k = 0; k < 3; k++) {
    std::vector<size_t> indices { k };
    line_view[indices] = k;
    EXPECT_THAT(line_view[indices], Eq(k));
  }
}

/*----------------------------------------------------------------------------*/

TEST_F(ALineView,
       ChangesAllCorrectPositionsOfContainerWhenChangingByJointIndices) {
  for (size_t k = 0; k < 3; k++) {
    std::vector<size_t> indices { k };
    line_view[indices] = k + 1;
  }

  for (size_t k = 0; k < 3; k++) {
    std::vector<size_t> indices { 0, 0, k };
    EXPECT_THAT(cube[indices], Ne(0));
  }

  for (size_t i = 1; i < 3; i++) {
    for (size_t j = 1; j < 3; j++) {
      for (size_t k = 0; k < 3; k++) {
        std::vector<size_t> indices { i, j, k };
        EXPECT_THAT(cube[indices], Eq(0));
      }
    }
  }
}

/*----------------------------------------------------------------------------*/

TEST_F(ALineView,
       DiesChangingIndexOutOfBoundInFirstDimensionWithJointIndices) {
  std::vector<size_t> indices { 4 };
  EXPECT_DEATH(line_view[indices] = 42, "check_indices");
}

/*----------------------------------------------------------------------------*/

TEST_F(ALineView, AllowsAccessingFieldsBySeparatedIndices) {
  for (size_t k = 0; k < 3; k++) {
    EXPECT_THAT(line_view[k], Eq(0));
  }
}

/*----------------------------------------------------------------------------*/

TEST_F(ALineView, AllowsChangingItsFieldsBySeparatedIndices) {
  for (size_t k = 0; k < 3; k++) {
    line_view[k] = k;
    EXPECT_THAT(line_view[k], Eq(k));
  }
}

/*----------------------------------------------------------------------------*/

TEST_F(ALineView,
       ChangesAllCorrectPositionsOfContainerWhenChangingBySeparatedIndices) {
  for (size_t k = 0; k < 3; k++) {
    line_view[k] = k + 1;
  }

  for (size_t k = 0; k < 3; k++) {
    EXPECT_THAT(cube[0][0][k], Ne(0));
  }

  for (size_t i = 1; i < 3; i++) {
    for (size_t j = 1; j < 3; j++) {
      for (size_t k = 0; k < 3; k++) {
        EXPECT_THAT(cube[i][j][k], Eq(0));
      }
    }
  }
}

/*----------------------------------------------------------------------------*/

TEST_F(ALineView,
       DiesChangingIndexOutOfBoundInFirstDimensionWithSeparatedIndices) {
  EXPECT_DEATH(line_view[4] = 42, "check_indices");
}

/*----------------------------------------------------------------------------*/
/*                           POINT MULTIVECTOR VIEW                           */
/*----------------------------------------------------------------------------*/

TEST_F(APointView, HasRightNumberOfDimensions) {
  ASSERT_THAT(point_view.num_dimensions(), Eq(0));
}

/*----------------------------------------------------------------------------*/

TEST_F(APointView, DiesGettingRangeOfNonExistentDimension) {
  ASSERT_DEATH(point_view.dimension_range(0), "check_dimension");
}

/*----------------------------------------------------------------------------*/

TEST_F(APointView, IsEqualToAnotherViewWithSameRangesAndValues) {
  multivector_t<int> another_cube { 3, 3, 3 };
  view_t<int> another_point_view { another_cube, { 0, 0, 0 }, {} };

  EXPECT_TRUE(point_view == another_point_view);
}

/*----------------------------------------------------------------------------*/

TEST_F(APointView, IsDifferentFromAViewWithMoreDimensions) {
  view_t<int> line_view { cube, { 0, 0 }, { range_t(0, 3) } };

  EXPECT_TRUE(point_view != line_view);
}

/*----------------------------------------------------------------------------*/

TEST_F(APointView, IsDifferentFromAViewWithSameDimensionsButDifferentValues) {
  multivector_t<int> another_cube { 3, 3, 3 };
  view_t<int> another_point_view { another_cube, { 0, 0 }, { range_t(0, 3) } };

  std::vector<size_t> indices { 0, 0, 0 };
  another_cube[indices] = 42;

  EXPECT_TRUE(point_view != another_point_view);
}

/*----------------------------------------------------------------------------*/

TEST_F(APointView, DiesTryingToGenerateAViewWithZeroIndices) {
  std::vector<size_t> indices {};
  ASSERT_DEATH(point_view[indices], "check_num_indices");
}

/*----------------------------------------------------------------------------*/

TEST_F(APointView, AllowsConvertingToValue) {
  EXPECT_THAT(point_view, Eq(0));
}

/*----------------------------------------------------------------------------*/

TEST_F(APointView, AllowsChangingValue) {
  point_view = 42;
  EXPECT_THAT(point_view, Eq(42));
}

/*----------------------------------------------------------------------------*/

TEST_F(APointView, ChangesAllCorrectPositionsOfContainerWhenChangingValue) {
  point_view = 42;

  EXPECT_THAT(cube[0][0][0], Ne(0));

  for (size_t i = 1; i < 3; i++) {
    for (size_t j = 1; j < 3; j++) {
      for (size_t k = 1; k < 3; k++) {
        EXPECT_THAT(cube[i][j][k], Eq(0));
      }
    }
  }
}

/*----------------------------------------------------------------------------*/
