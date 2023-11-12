package io.qaload.gatling.reportExample.simulation

import io.gatling.core.Predef.{nothingFor, _}
import io.qaload.gatling.reportExample.process.SimpleScenario
import io.qaload.gatling.reportExample.setting.Protocol

import scala.concurrent.duration._

/**
 * Meta DSL to write open increment tests (succession of several increasing levels)
 *
 * incrementUsersPerSec(incrementUsersPerSec)
 * .times(numberOfSteps)
 * .eachLevelLasting(levelDuration)
 * .separatedByRampsLasting(rampDuration)
 * .startingFrom(initialUsersPerSec)
 *
 * Inject a succession of numberOfSteps levels each one during levelDuration
 * and increasing the number of users per sec by incrementUsersPerSec
 * starting from zero or the optional initialUsersPerSec
 * and separated by optional ramps lasting rampDuration
 *
 *                                         nbOfSteps - 1  ==  5 - 1  ==  4
 *                                                                 +-----------+ == startingUsers + usersPerSec * 4
 *                                                                /.           |    200           + 500         * 4
 *                                                        3      / .           |
 *                                                  +-----------+  -  -  -  -  | == startingUsers + usersPerSec * 3
 *                                                 /.           .  .           |    200           + 500         * 3
 *                                         2      / .           .  .           |
 *                                   +-----------+  -  -  -  -  -  -  -  -  -  | == startingUsers + usersPerSec * 2
 *                                  /.           .  .           .  .           |    200           + 500         * 2
 *                          1      / .           .  .           .  .           |
 *                    +-----------+  -  -  -  -  -  -  -  -  -  -  -  -  -  -  | == startingUsers + usersPerSec
 *                   /.           .  .           .  .           .  .           |    200           + 500
 *           0      / .           .  .           .  .           .  .           |
 *     +-----------+  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  | == startingUsers
 *     |           .  .           .  .           .  .           .  .           |    200
 *   +_+           .  .           .  .           .  .           .  .           +_+
 *   0 #############**#############**#############**#############**############# 1
 */
class OpenModel_IncrementUsersPerSec extends Simulation {
  setUp(

    SimpleScenario.simpleScenario("none").inject(
      atOnceUsers(1),                                              // 0
      nothingFor(5 seconds),
    ).protocols(Protocol.localHttpConf),

    SimpleScenario.simpleScenario("incrementUsersPerSec").inject(  //
      incrementUsersPerSec(500)                               // usersPerSec
        .times(5)                                              // nbOfUsers
        .eachLevelLasting(60 seconds)                                    // #############
        .separatedByRampsLasting(5 seconds)                     // **
        .startingFrom(200),                                    // startingUsers

      nothingFor(5 seconds),
      atOnceUsers(1)                                               // 1
    ).protocols(Protocol.httpConf)
  )
}
