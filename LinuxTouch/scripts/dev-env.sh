# Shared config for the dev scripts. Sourced, not executed.
#
# WHY NOTHING HERE HAS A DEFAULT
# ------------------------------
# A board's address belongs to the bench it sits on. Baking one into a tracked
# file is wrong twice over: every other clone of this repo gets a value that is
# useless to them at best and points at a stranger's machine at worst, and the
# author's network layout ships to whatever remote the repo is pushed to. The
# same habit is how credentials end up in git history.
#
# So the address is ASKED FOR on first use and written to scripts/dev.env,
# which is gitignored. Resolution order, most explicit first:
#
#   1. --device user@host        (a script argument; also --key, --port)
#   2. an environment variable   (LINUXTOUCH_DEVICE=user@host make deploy)
#   3. scripts/dev.env           (gitignored, per-workstation)
#   4. an interactive prompt     (offers to save for next time)
#   5. non-interactive with none of the above: fail with instructions
#
# Steps 1 and 2 exist because a prompt is not always possible — CI, a cron job,
# a `make` invocation with stdin redirected. Guessing never is.
#
# Secrets are never prompted for or stored here. SSH keys are referenced by
# PATH, and the board's sudo password is typed into sudo's own prompt on the
# board.

# shellcheck disable=SC2128
_here="$(cd "$(dirname "${BASH_SOURCE:-$0}")" && pwd)"
_envfile="$_here/dev.env"

while [ $# -gt 0 ]; do
    case "$1" in
        --device)   LINUXTOUCH_DEVICE="${2:?--device needs a value}"; shift 2 ;;
        --device=*) LINUXTOUCH_DEVICE="${1#*=}"; shift ;;
        --key)      LINUXTOUCH_KEY="${2:?--key needs a value}"; shift 2 ;;
        --key=*)    LINUXTOUCH_KEY="${1#*=}"; shift ;;
        --port)     LINUXTOUCH_PORT="${2:?--port needs a value}"; shift 2 ;;
        --port=*)   LINUXTOUCH_PORT="${1#*=}"; shift ;;
        *)          break ;;
    esac
done

if [ -f "$_envfile" ]; then
    # An explicit environment variable beats the file, so save anything already
    # set and put it back afterwards.
    _pre_device="${LINUXTOUCH_DEVICE:-}"
    _pre_key="${LINUXTOUCH_KEY:-}"
    _pre_port="${LINUXTOUCH_PORT:-}"
    # shellcheck disable=SC1090
    . "$_envfile"
    [ -n "$_pre_device" ] && LINUXTOUCH_DEVICE="$_pre_device"
    [ -n "$_pre_key" ]    && LINUXTOUCH_KEY="$_pre_key"
    [ -n "$_pre_port" ]   && LINUXTOUCH_PORT="$_pre_port"
fi

if [ -z "${LINUXTOUCH_DEVICE:-}" ]; then
    if [ ! -t 0 ]; then
        cat >&2 <<EOF
No board configured, and no terminal to ask on.

Pass it explicitly:
    make deploy DEVICE=user@host

or set it in the environment:
    LINUXTOUCH_DEVICE=user@host make deploy

or create scripts/dev.env from scripts/dev.env.example.
EOF
        exit 1
    fi

    echo "No board configured yet — scripts/dev.env does not exist."
    echo
    printf 'Board SSH destination (user@host): '
    read -r _in_device
    [ -n "$_in_device" ] || { echo "Nothing entered." >&2; exit 1; }
    printf 'SSH key [~/.ssh/id_ed25519]: '
    read -r _in_key
    [ -n "$_in_key" ] || _in_key="~/.ssh/id_ed25519"
    printf 'Save these to scripts/dev.env for next time? [Y/n] '
    read -r _in_save
    case "$_in_save" in
        [Nn]*) ;;
        *)
            # 0600: it holds no secret today, but it is per-user config for
            # reaching a machine and nothing else here needs to read it.
            (umask 077; cat > "$_envfile" <<EOF
# Local dev board config. GITIGNORED — do not commit, and do not add
# credentials here. See dev.env.example for the full list of settings.
LINUXTOUCH_DEVICE=$_in_device
LINUXTOUCH_KEY=$_in_key
EOF
            )
            echo "Wrote $_envfile (gitignored)."
            ;;
    esac
    echo
    LINUXTOUCH_DEVICE="$_in_device"
    LINUXTOUCH_KEY="$_in_key"
fi

DEVICE="$LINUXTOUCH_DEVICE"
# eval so a leading ~ expands; the file is config, not a shell context.
eval KEY="${LINUXTOUCH_KEY:-~/.ssh/id_ed25519}"
PORT="${LINUXTOUCH_PORT:-8720}"
SSH_OPTS="-o IdentitiesOnly=yes -o ConnectTimeout=8 -i $KEY"

case "$0" in
  */dev-env.sh|dev-env.sh)
    echo "device   $DEVICE"
    echo "key      $KEY"
    echo "port     $PORT"
    echo "config   $_envfile$([ -f "$_envfile" ] || echo ' (does not exist)')"
    echo
    echo "This file is meant to be sourced by the dev scripts, not run."
    ;;
esac
