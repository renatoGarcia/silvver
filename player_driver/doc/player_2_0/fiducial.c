#include <stdio.h>
#include <libplayerc/playerc.h>

int main(int argc, const char **argv)
{
  playerc_client_t *client;
  playerc_fiducial_t *fiducial;

  client = playerc_client_create(NULL, "localhost", 6665);
  if(playerc_client_connect(client) != 0)
  {
    fprintf(stderr, "error: %s\n", playerc_error_str());
    return -1;
  }
  fiducial = playerc_fiducial_create(client, 0);
  if(playerc_fiducial_subscribe(fiducial, PLAYERC_OPEN_MODE) != 0)
  {
    fprintf(stderr, "fiducial subscribe error: %s\n", playerc_error_str());
    return -1;
  }

  if(playerc_client_datamode(client, PLAYERC_DATAMODE_PULL) != 0)
  {
    fprintf(stderr, "error: %s\n", playerc_error_str());
    return -1;
  }
  if(playerc_client_set_replace_rule(client, -1, -1,
                                     PLAYER_MSGTYPE_DATA, -1, 1) != 0)
  {
    fprintf(stderr, "error: %s\n", playerc_error_str());
    return -1;
  }

  int i,j;
  for (i = 0; i < 500; ++i)
  {
    playerc_client_read(client);
    for (j = 0; j < fiducial->fiducials_count; j++)
    {
      printf("fiducial (id, x,y,theta): %d %f %f %f\n",
             fiducial->fiducials[j].id,
             fiducial->fiducials[j].pose.px,
             fiducial->fiducials[j].pose.py,
             fiducial->fiducials[j].pose.pyaw);
    }
    if (j != 0) /* If printed anything above */
    {
      printf("\n");
    }
    fflush(stdout);
  }

  /* Shutdown and tidy up */
  playerc_fiducial_unsubscribe(fiducial);
  playerc_fiducial_destroy(fiducial);
  playerc_client_disconnect(client);
  playerc_client_destroy(client);

  return 0;
}
