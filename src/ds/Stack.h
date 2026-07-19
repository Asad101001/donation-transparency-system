#ifndef STACK_H
#define STACK_H

#include "../models/Models.h"

void pushStatus(Donation* d, DonationStatus newStatus);
DonationStatus popStatus(Donation* d);
void printStatusHistory(Donation* d);

#endif // STACK_H
