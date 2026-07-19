#pragma once
#include "../models/Models.h"

void pushStatus(Donation* d, DonationStatus newStatus);
DonationStatus popStatus(Donation* d);
void printStatusHistory(Donation* d);
