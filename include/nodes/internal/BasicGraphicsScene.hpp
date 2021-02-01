#pragma once

#include <QtCore/QUuid>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QMenu>

#include <functional>
#include <memory>
#include <tuple>
#include <unordered_map>

#include "Definitions.hpp"
#include "Export.hpp"
#include "GraphModel.hpp"
#include "ConnectionIdHash.hpp"

#include "QUuidStdHash.hpp"

namespace QtNodes
{

class ConnectionGraphicsObject;
class GraphModel;
class NodeGraphicsObject;
class NodeStyle;

/// An instance of QGraphicsScene, holds connections and nodes.
class NODE_EDITOR_PUBLIC BasicGraphicsScene : public QGraphicsScene
{
  Q_OBJECT
public:

  BasicGraphicsScene(GraphModel &graphModel,
                     QObject *    parent = nullptr);

  // Scenes without models are not supported
  BasicGraphicsScene() = delete;

  ~BasicGraphicsScene();

public:

  /// @returns associated GraphModel.
  GraphModel const &
  graphModel() const;

  GraphModel &
  graphModel();

public:

  /// Creates a "draft" instance of ConnectionGraphicsObject.
  /**
   * The scene caches a "draft" connection which has one loose end.
   * After attachment the "draft" instance is deleted and instead a
   * normal "full" connection is created.
   * Function @returns the "draft" instance for further geometry
   * manipulations.
   */
  std::unique_ptr<ConnectionGraphicsObject> const &
  makeDraftConnection(ConnectionId const newConnectionId);

  /// Deletes "draft" connection.
  /**
   * The function is called when user releases the mouse button during
   * the construction of the new connection without attaching it to any
   * node.
   */
  void
  resetDraftConnection();

  /// Deletes all the nodes. Connections are removed automatically.
  void
  clearScene();

public:

  /// @returns NodeGraphicsObject associated with the given nodeId.
  /**
   * @returns nullptr when the object is not found.
   */
  NodeGraphicsObject *
  nodeGraphicsObject(NodeId nodeId);

  /// @returns ConnectionGraphicsObject corresponding to `connectionId`.
  /**
   * @returns `nullptr` when the object is not found.
   */
  ConnectionGraphicsObject *
  connectionGraphicsObject(ConnectionId connectionId);

public:

  /// Can @return an instance of the scene context menu in subclass.
  /**
   * Default implementation returns `nullptr`.
   */
  virtual
  QMenu *
  createSceneMenu(QPointF const scenePos);

Q_SIGNALS:

  void
  nodeMoved(NodeId const nodeId, QPointF const & newLocation);

  void
  nodeDoubleClicked(NodeId const nodeId);

  void
  nodeHovered(NodeId const nodeId, QPoint const screenPos);

  void
  nodeHoverLeft(NodeId const nodeId);

  void
  connectionHovered(ConnectionId const connectionId, QPoint const screenPos);

  void
  connectionHoverLeft(ConnectionId const connectionId);

  /// Signal allows showing custom context menu upon clicking a node.
  void
  nodeContextMenu(NodeId const nodeId, QPointF const pos);

private:

  /// @brief Creates Node and Connection graphics objects.
  /**
   * Function is used to populate an empty scene in the constructor. We
   * perform depth-first GraphModel traversal. The connections are
   * created by checking non-empty node `Out` ports.
   */
  void
  traverseGraphAndPopulateGraphicsObjects();

  /// Redraws adjacent nodes for given `connectionId`
  void
  updateAttachedNodes(ConnectionId const connectionId,
                      PortType const portType);

private Q_SLOTS:


  /// Slot called when the `connectionId` is erased form the GraphModel.
  void
  onConnectionDeleted(ConnectionId const connectionId);

  /// Slot called when the `connectionId` is created in the GraphModel.
  void
  onConnectionCreated(ConnectionId const connectionId);

  void
  onNodeDeleted(NodeId const nodeId);

  void
  onNodeCreated(NodeId const nodeId);

  void
  onNodePositionUpdated(NodeId const nodeId);

  void
  onPortsAboutToBeDeleted(NodeId const nodeId,
                          PortType const portType,
                          std::unordered_set<PortIndex> const &portIndexSet);

  void
  onPortsDeleted(NodeId const nodeId,
                 PortType const portType,
                 std::unordered_set<PortIndex> const &portIndexSet);

  void
  onPortsAboutToBeInserted(NodeId const nodeId,
                           PortType const portType,
                           std::unordered_set<PortIndex> const &portIndexSet);

  void
  onPortsInserted(NodeId const nodeId,
                  PortType const portType,
                  std::unordered_set<PortIndex> const &portIndexSet);


private:

  // TODO shared pointer?
  GraphModel &_graphModel;


  using UniqueNodeGraphicsObject =
    std::unique_ptr<NodeGraphicsObject>;

  using UniqueConnectionGraphicsObject =
    std::unique_ptr<ConnectionGraphicsObject>;

  std::unordered_map<NodeId, UniqueNodeGraphicsObject>
  _nodeGraphicsObjects;

  std::unordered_map<ConnectionId,
                     UniqueConnectionGraphicsObject>
  _connectionGraphicsObjects;


  std::unique_ptr<ConnectionGraphicsObject> _draftConnection;
};


}
