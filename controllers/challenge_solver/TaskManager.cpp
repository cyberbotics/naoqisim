#include <iostream>
#include <stdexcept>
#include "TaskManager.hpp"
#include "const.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

TaskManager::TaskManager(string naoIp, int naoPort) {
  mBroker = AL::ALBroker::createBroker("resolverBroker", "0.0.0.0", 54000, naoIp, naoPort);
  mVisionManager = new VisionManager(mBroker);
  mMovementManager = new MovementManager(mBroker);
}

TaskManager::~TaskManager() {
  delete mVisionManager;
  delete mMovementManager;
}

void TaskManager::run() {
  cout << "Performing task 1: putting the key in the box." << endl;
  moveKeyInPlace();

  mMovementManager->followupTask();

  cout << "Performing task 2: activating the distributor." << endl;
  turnDistributorOn();

  mMovementManager->followupTask();

  cout << "Performing task 3: pointing the date on the calendar." << endl;
  pointCalendar();
}


void TaskManager::moveKeyInPlace() {
  bool startOver = false;
  int cnt = 0;
  double epsilon = 0.01;

  // Step: Search for the key
  do {
    if (cnt == MAX_RUN)
      throw logic_error("Couldn't find the key in any door.");

    startOver = false;

    cout << "Searching for the white line ..." << endl;
    searchLine();

    cout << "Following the line ..." << endl;
    while (abs(mVisionManager->findOrientation()) < 0.15)
      mMovementManager->walk(0.0, -0.8);
    mMovementManager->stopMoving();

    while(!mMovementManager->obstacleDetected())
      followLine();
    mMovementManager->stopMoving();
    mMovementManager->look("front");

    // If the door contains a key, it's the right one,
    // otherwise, start again in the other direction.
    if (!mVisionManager->seeColor(RED, TOP_CAM)) {
      mMovementManager->look("left");
      if (!mVisionManager->seeColor()) {
        mMovementManager->look("right");
        if (!mVisionManager->seeColor()) {
          cout << "Wrong door. Following the line the other way around ..." << endl;
          mMovementManager->turn(3.1415);
          startOver = true;
        }
      }
      mMovementManager->look("front");
    }

    ++cnt;
  } while (startOver);
  cout << "The key has been detected." << endl;

  // Step: pick up the key
  cout << "Calibrating Nao according to the key... (This can take quite some time)" << endl;
  calibration(2 * epsilon, 15, "Key", true);
  mMovementManager->prepareArm();
  approachColor(0.1, 0.15, false);
  calibration(1.5 * epsilon, 20, "Key", true); // protection against deviations
  approachColor(0.1, 0.35, false);
  calibration(1.5 * epsilon, 50, "Key", true); // protection against deviations
  mMovementManager->calibrate(0.03);

  cout << "Nao is ready to pick up the key." << endl;
  mMovementManager->pickupKey();
  if (!mVisionManager->hasKeyInHand())
    throw logic_error("Nao failed to get the key.");
  mMovementManager->handleKey();
  cout << "Nao got the key. On his way to set it in place..." << endl;

  // Step: search for the box along the path and drop the key in it
  searchItem("Box");
  calibration(5 * epsilon, 15, "Box", false);
  mMovementManager->pointFist(-0.25);
  mMovementManager->look("down");
  approachColor(0.6, 0.2, true);
  calibration(4 * epsilon, 20, "Box", false); // protection against deviations
  approachColor(0.6, 0.3, false);
  calibration(2 * epsilon, 50, "Box", false); // protection against deviations
  mMovementManager->calibrate(0.15);
  mMovementManager->prepareKey();
  mMovementManager->releaseKey();
  mMovementManager->look("front");

  cout << "Task 1 completed." << endl;
}

void TaskManager::turnDistributorOn() {
  double epsilon = 0.05;

  // Step: search for the distributor along the path
  searchItem("Distributor");

  // Step: Press on the button
  cout << "Preparing to press the button ..." << endl;
  approachColor(0.6, 0.175, true);
  calibration(2 * epsilon, 10, "Distributor", false);
  approachColor(0.6, 0.3, false);
  calibration(epsilon, 10, "Distributor", false);
  mMovementManager->pressButton();

  int iter = 0;
  while (!mVisionManager->seeColor(LIGHT_GREEN, TOP_CAM)) {
    mMovementManager->walk(0.2, 0);
    ++iter;
    if (iter == 50)    {
      if (!findItem("Distributor") && !mVisionManager->seeColor(LIGHT_GREEN, TOP_CAM))
        throw logic_error("Nao failed to activate the distributor.");
      iter = 0;
    }
  }
  cout << "The distributor has been activated!" << endl;
  mMovementManager->stopMoving();

  cout << "Task 2 completed." << endl;
}

void TaskManager::pointCalendar() {
  int vertOffset = 0;
  int horOffset = 0;
  double epsilon = 0.025;

  // Step: Search for the calendar along the path
  searchItem("Calendar");
  approachColor(0.6, 0.1, true);
  calibration(epsilon, 10, "Calendar", true);

  // Step: Point the current day
  time_t rawtime;
  tm *timeinfo;
  time(&rawtime);
  timeinfo=localtime(&rawtime);
  int wday=timeinfo->tm_wday;

  switch (wday) {
    case 0: {
      vertOffset = -1;
      cout << "It's sunday!" << endl;
      break;
    }
    case 1: {
      vertOffset = 1;
      horOffset = 1;
      cout << "It's monday!" << endl;
      break;
    }
    case 2: {
      horOffset = 2;
      cout << "It's tuesday!" << endl;
      break;
    }
    case 3: {
      horOffset = 1;
      cout << "It's wednessday!" << endl;
      break;
    }
    case 4: {
      cout << "It's thursday!" << endl;
      break;
    }
    case 5: {
      vertOffset = -1;
      horOffset = 2;
      cout << "It's friday!" << endl;
      break;
    }
    case 6: {
      vertOffset = -1;
      horOffset = 1;
      cout << "It's saturday!" << endl;
      break;
    }
    default: {
      cout << "Nao is sad to announce that he doesn't know which day it is." << endl;
      break;
    }
  }

  if (vertOffset == -1)
    mMovementManager->look("down");
  mMovementManager->calibrate(0.18 * horOffset);
  mMovementManager->pointFist(vertOffset == -1 ? 0.3 : vertOffset == 1 ? -0.52 : -0.1);

  cout << "Task 3 completed." << endl;
}

void TaskManager::searchLine() {
  while (!mVisionManager->seeColor(WHITE, BOT_CAM))
    mMovementManager->searchOnGround();
  mMovementManager->endSearch();
}

void TaskManager::calibration(double epsilon, double offsetRatio, string item, bool align) {
  int cnt = 0;
  int iter = 0;
  double offset = 0.0;
  string itemLC = item;
  transform(itemLC.begin(), itemLC.end(), itemLC.begin(), ::tolower);

  do {
    if (cnt == MAX_CALIBRATION_ATTEMPT)
      throw logic_error("Error: Nao failed to calibrate with the " + itemLC + ".");

    if (align)
      mMovementManager->alignWithObstacle();

    if (!mVisionManager->seeColor()) {
      offset = 0.1;
      cout << item << " lost. Searching for it ..." << endl;

      mMovementManager->look("left");
      if (!mVisionManager->seeColor()) {
        mMovementManager->look("right");
        if (!mVisionManager->seeColor())
          throw logic_error("Error: the " + itemLC + " can't be found anymore.");
        offset = -0.1;
      }
      mMovementManager->look("front");

      int sidestepCnt = 0;
      while (!mVisionManager->seeColor()) {
        if (sidestepCnt == MAX_SIDESTEP_ATTEMPT)
          throw logic_error("Error: Nao failed to get back to the " + itemLC + ".");

        mMovementManager->calibrate(offset);
        ++sidestepCnt;
      }
    }

    int scaling = 0;
    while (abs(offset = mVisionManager->findOrientation()) > epsilon) {
      ++iter;
      mMovementManager->calibrate(offset / (offsetRatio * ((scaling / 2) + 1)));
      if (iter == 10) {
        if (align)
          mMovementManager->alignWithObstacle();
        if (scaling < 6)
          ++scaling;
        iter = 0;
      }
    }

    ++cnt;
  } while (!mVisionManager->seeColor());
}

void TaskManager::approachColor(double velocity, double visionRequired, bool correction) {
  int iter = 0;
  double epsilon = 0.05;
  double orientation = 0.0;

  mMovementManager->walk(velocity, 0);
  while (!mVisionManager->seeEnoughColor(visionRequired)) {
    ++iter;
    if (iter == 25) {
      if (!mVisionManager->seeColor()) {
        mMovementManager->stopMoving();
        throw logic_error("Nao lost track of where he was going.");
      } else if (correction && !(abs(mVisionManager->findOrientation()) < 0.25)) {
        mMovementManager->stopMoving();
        while (!(abs(orientation = mVisionManager->findOrientation()) < epsilon))
          mMovementManager->walk(0, orientation);
        mMovementManager->stopMoving();
        mMovementManager->walk(velocity, 0);
      }
      iter = 0;
    }
  }
  mMovementManager->stopMoving();
}

void TaskManager::searchItem(string item) {
  if ((item != "Box") && (item != "Distributor") && (item != "Calendar")) {
    cerr << "Error: function searchItem(string item) expects inputs \
            \"Box\", \"Distributor\", or \"Calendar\"." << endl;
    return;
  }

  bool startOver = false;
  bool checkAgain = false;
  bool found = false;
  int cnt = 0;
  int posHead = 0;
  double orientation = 0.0;
  double lCap = 0.05;
  double uCap = item == "Box" ? 0.15 : 0.25;
  double epsilon = 0.05;
  string itemLC = item;
  transform(itemLC.begin(), itemLC.end(), itemLC.begin(), ::tolower);

  do {
    startOver = false;

    // Step: search for the path on the ground
    cout << "Searching for the white line ..." << endl;
    searchLine();

    // Step: follow the path until finding the item
    cout << "Following the line ..." << endl;
    while (abs(mVisionManager->findOrientation()) < 0.15)
      mMovementManager->walk(0.0, -0.8);
    mMovementManager->stopMoving();

    while(!mMovementManager->obstacleDetected() && !found) {
      orientation = followLine();
      if (!checkAgain && (abs(orientation) < lCap))
        checkAgain = true;
      if(checkAgain && abs(orientation) > uCap) {
        mMovementManager->stopMoving();
        cout << "Looking for the " << itemLC << " ..." << endl;
        posHead = 0;
        while (!found && posHead < 6) {
          switch (posHead) {
            case 0: mMovementManager->look("right+"); break;
            case 1: mMovementManager->look("right"); break;
            case 2: mMovementManager->look("front"); break;
            case 3: mMovementManager->look("left"); break;
            case 4: mMovementManager->look("left+"); break;
            default: {
              cout << item << " not found. Following the line ..." << endl;
              checkAgain = false;
              mMovementManager->look("down");
              mVisionManager->seeColor(WHITE, BOT_CAM);
#ifdef _WIN32
              Sleep(1000);
#else
              usleep(1000000);
#endif
              break;
            }
          }
          if (posHead < 5)
            found = findItem(item);
          if (!found)
            ++posHead;
        }
      }
    }

    if (found) {
      // The item has been found: look in its direction
      cout << item << " found!" << endl;

      mMovementManager->look("front");
      switch(posHead) {
        case 0:
        case 1: orientation = -1.0; break;
        case 2: break;
        case 3:
        case 4: orientation = 1.0; break;
        default: throw logic_error("Unexpected behavior.");
      }

      while (!findItem(item))
        mMovementManager->walk(0, orientation);
      mMovementManager->stopMoving();

      mVisionManager->seeColor(RED, TOP_CAM);
      while (!(abs(orientation = mVisionManager->findOrientation()) < epsilon))
        mMovementManager->walk(0, orientation);
      mMovementManager->stopMoving();
    } else {
      // We reached an extremity of the line without finding the object
      mMovementManager->stopMoving();

      ++cnt;
      if (cnt == MAX_RUN)
        throw logic_error("Couldn't find the " + itemLC + ".");

      startOver = true;

      cout << item << " not found. Following the line the other way around ..." << endl;
      mMovementManager->turn(3.1415);
    }
  } while (startOver);
}

bool TaskManager::findItem(string item) {
  if (item == "Box") {
    if (mVisionManager->seeEnoughColor(RED, TOP_CAM, 0.0465) &&
        mVisionManager->seeColor(DARK_BROWN, TOP_CAM) &&
        !mVisionManager->seeColor(BLUE, TOP_CAM) &&
        !mVisionManager->seeEnoughColor(YELLOW, TOP_CAM, 0.0005))
      return true;
  } else if (item == "Distributor") {
    if (mVisionManager->seeEnoughColor(BLACK, TOP_CAM, 0.03) &&
        mVisionManager->seeEnoughColor(RED, TOP_CAM, 0.05) &&
        !mVisionManager->seeColor(BLUE, TOP_CAM) &&
        !mVisionManager->seeEnoughColor(DARK_BROWN, TOP_CAM, 0.0005))
      return true;
  } else if (item == "Calendar") {
    if (mVisionManager->seeColor(RED, TOP_CAM) &&
        mVisionManager->seeEnoughColor(BLUE, TOP_CAM, 0.08) &&
        !mVisionManager->seeColor(DARK_BROWN, TOP_CAM))
      return true;
  } else {
    cerr << "Error: function findItem(string item) expects inputs \
            \"Box\", \"Distributor\", or \"Calendar\"." << endl;
  }

  return false;
}

double TaskManager::followLine() {
  double orientation = 0.0;
  mMovementManager->walk(0.8, orientation = mVisionManager->findOrientation());
  if (!orientation && !mVisionManager->seeColor(WHITE, BOT_CAM)) {
    mMovementManager->stopMoving();
    cout << "Line lost of sight. Going backwards ..." << endl;
    while (!mVisionManager->seeEnoughColor(0.05))
      mMovementManager->walk(-0.6, 0.0);
    mMovementManager->stopMoving();
    mMovementManager->walk(0.8, orientation = mVisionManager->findOrientation());
  }

  return orientation;
}
