/*
 * This software is distributed under BSD 3-clause license (see LICENSE file).
 *
 * Authors: Soeren Sonnenburg, Sergey Lisitsyn, Evan Shelhamer
 */

#include <shogun/evaluation/MulticlassAccuracy.h>
#include <shogun/labels/Labels.h>
#include <shogun/labels/MulticlassLabels.h>
#include <shogun/mathematics/Math.h>

using namespace shogun;

float64_t CMulticlassAccuracy::evaluate(CLabels* predicted, CLabels* ground_truth)
{
	ASSERT(predicted && ground_truth)
	ASSERT(predicted->get_num_labels() == ground_truth->get_num_labels())
	ASSERT(predicted->get_label_type() == LT_MULTICLASS)
	ASSERT(ground_truth->get_label_type() == LT_MULTICLASS)
	int32_t length = predicted->get_num_labels();
	int32_t correct = 0;
	if (m_ignore_rejects)
	{
		for (int32_t i=0; i<length; i++)
		{
			if (((CMulticlassLabels*) predicted)->get_int_label(i)==((CMulticlassLabels*) ground_truth)->get_int_label(i))
				correct++;
		}
		return ((float64_t)correct)/length;
	}
	else
	{
		int32_t total = length;
		for (int32_t i=0; i<length; i++)
		{
			int32_t predicted_label = ((CMulticlassLabels*) predicted)->get_int_label(i);

			if (predicted_label==((CMulticlassLabels*) predicted)->REJECTION_LABEL)
				total--;
			else if (predicted_label==((CMulticlassLabels*) ground_truth)->get_int_label(i))
				correct++;
		}
		m_rejects_num = length-total;
		SG_DEBUG("correct=%d, total=%d, rejected=%d\n",correct,total,length-total)
		return ((float64_t)correct)/total;
	}
	return 0.0;
}

SGMatrix<int32_t> CMulticlassAccuracy::get_confusion_matrix(CLabels* predicted, CLabels* ground_truth)
{
	ASSERT(predicted->get_num_labels() == ground_truth->get_num_labels())
	int32_t length = ground_truth->get_num_labels();
	int32_t num_classes = ((CMulticlassLabels*) ground_truth)->get_num_classes();
	SGMatrix<int32_t> confusion_matrix(num_classes, num_classes);
	memset(confusion_matrix.matrix,0,sizeof(int32_t)*num_classes*num_classes);
	for (int32_t i=0; i<length; i++)
	{
		int32_t predicted_label = ((CMulticlassLabels*) predicted)->get_int_label(i);
		int32_t ground_truth_label = ((CMulticlassLabels*) ground_truth)->get_int_label(i);

		if (predicted_label==((CMulticlassLabels*) predicted)->REJECTION_LABEL)
			continue;

		confusion_matrix[predicted_label*num_classes+ground_truth_label]++;
	}
	return confusion_matrix;
}

